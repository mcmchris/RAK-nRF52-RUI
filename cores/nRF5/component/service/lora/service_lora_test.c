#include "service_lora_test.h"
#include <stddef.h>
#include <stdint.h>
#include "udrv_errno.h"
#include "udrv_serial.h"
#include "board_basic.h"
#include "service_nvm.h"
#include "LoRaMac.h"
#include "Region.h"
#include "radio.h"
#include "delay.h"

#define TX_TEST_TONE (1 << 0)
#define RX_TEST_RSSI (1 << 1)
#define TX_TEST_LORA (1 << 2)
#define RX_TEST_LORA (1 << 3)
#define RX_TIMEOUT_VALUE 5000

// #define TEST_MODEM_FSK                              0
// #define TEST_MODEM_LORA                             1

#define TX_OUTPUT_POWER 14      // dBm
#define LORA_BANDWIDTH 0        // [0: 125 kHz, \
                                //  1: 250 kHz, \
                                //  2: 500 kHz, \
                                //  3: Reserved]
#define LORA_SPREADING_FACTOR 7 // [SF7..SF12]
#define LORA_CODINGRATE 1       // [1: 4/5, \
                                //  2: 4/6, \
                                //  3: 4/7, \
                                //  4: 4/8]
#define LORA_PREAMBLE_LENGTH 8  // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT 5   // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON false

#define PAYLOAD_LEN 4

#define FSK_FDEV 25000          // Hz
#define FSK_DATARATE 50000      // bps
#define FSK_BANDWIDTH 50000     // Hz
#define FSK_AFC_BANDWIDTH 83333 // Hz
#define FSK_PREAMBLE_LENGTH 5   // Same for Tx and Rx
#define FSK_FIX_LENGTH_PAYLOAD_ON false

static void OnTxDone(void);
static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);
static void OnTxTimeout(void);
static void OnRxTimeout(void);
static void OnRxError(void);
static int32_t Prbs9_generator(uint8_t *payload, uint8_t len);
static void OnTxTimerEvent(void);
static void Recv_Enent(void);
static RadioEvents_t RadioEvents;
static uint8_t TestState = 0;
extern TimerEvent_t CertifiTimer;

/*
* \param [IN] bandwidth    Sets the bandwidth
 *                          FSK : >= 2600 and <= 250000 Hz
 *                          LoRa: [0: 125 kHz, 1: 250 kHz,
 *                                 2: 500 kHz, 3: Reserved]
 * \param [IN] datarate     Sets the Datarate
 *                          FSK : 600..300000 bits/s
 *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                10: 1024, 11: 2048, 12: 4096  chips]
*/

static testParameter_t testParam = {MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                    FSK_AFC_BANDWIDTH, LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON, PAYLOAD_LEN,
                                    false, false, 0, 0,
                                    true, LORA_SYMBOL_TIMEOUT, TX_OUTPUT_POWER, FSK_FDEV,
                                    3000, 868000000, 0, 0,
                                    0, 0, 868000000, 868500000,
                                    100000, 6}; //Set a default value

static __IO int16_t last_rx_rssi = 0;
static __IO int8_t last_rx_snr = 0;
static __IO uint32_t packet = 0;
static __IO uint8_t hop_flag = 0;
static TimerEvent_t TxTimer;
static uint8_t payload[256] = {0};
static uint32_t Rx_count_RxKo;
static uint32_t Rx_count_RxOk;
static uint32_t freq_start_back;
static uint32_t packet_back;

uint32_t service_lora_ttone(void)
{
  if ((TestState & TX_TEST_TONE) != TX_TEST_TONE)
  {
    TestState |= TX_TEST_TONE;
    Radio.SetTxContinuousWave(testParam.frequency, testParam.power, 0xFFFF);
    return UDRV_RETURN_OK;
  }
  else
  {
    return -UDRV_INTERNAL_ERR;
  }
}

uint32_t service_lora_toff(void)
{
  TestState = 0;
  TimerStop(&TxTimer);
  TimerStop(&CertifiTimer);
  Radio.Sleep();
  return UDRV_RETURN_OK;
}

int16_t service_lora_trssi(void)
{
  uint32_t timeout = 0;
  int16_t rssiVal = 0;
  /* Test with LNA */
  /* check that test is not already started*/
  if ((TestState & RX_TEST_RSSI) != RX_TEST_RSSI)
  {
    TestState |= RX_TEST_RSSI;
    Radio.SetChannel(testParam.frequency);
    /* RX Continuous */
    Radio.SetTxConfig(testParam.modem, testParam.power, testParam.fdev, testParam.bandwidth, testParam.datarate, testParam.coderate, testParam.preambleLen,
                      testParam.fixLen, testParam.crcOn, testParam.FreqHopOn, testParam.HopPeriod, testParam.iqInverted, testParam.txTimeout);
    timeout = 0xFFFFFF; /* continuous Rx */
    if (testParam.lna == 0)
    {
      Radio.Rx(timeout);
    }
    else
    {
      Radio.RxBoosted(timeout);
    }
    DelayMs(50); /* Wait for 100ms */
    rssiVal = Radio.Rssi(MODEM_FSK);
    Radio.Sleep();
    TestState &= ~RX_TEST_RSSI;
    return rssiVal;
  }
  else
  {
    return -UDRV_INTERNAL_ERR;
  }
}

int32_t service_lora_set_tconf(testParameter_t *Param)
{
  if (Param->frequency < 150000000 || Param->frequency > 960000000)
  {
    return -UDRV_WRONG_ARG;
  }

  if (Param->power > 22 || Param->power < -9)
  {
    return -UDRV_WRONG_ARG;
  }

  if (Param->bandwidth > 6)
  {
    return -UDRV_WRONG_ARG;
  }

  if (Param->datarate < 5 || Param->datarate > 12)
  {
    return -UDRV_WRONG_ARG;
  }

  if (Param->coderate < 1 || Param->coderate > 4)
  {
    return -UDRV_WRONG_ARG;
  }

  if (1) //lna
  {
  }

  if (1) //paBoost_state
  {
  }

  if (Param->modem > 2)
  {
    return -UDRV_WRONG_ARG;
  }

  if (Param->payloadLen > 256)
  {
    return -UDRV_WRONG_ARG;
  }

  if (Param->fdev < 4800 || Param->fdev > 467000)
  {
    return -UDRV_WRONG_ARG;
  }

  if (1) //lowDrOpt
  {
    //return -UDRV_WRONG_ARG;
  }

  if (1) //BTproduct
  {
    //return -UDRV_WRONG_ARG;
  }

  memcpy1(&testParam, Param, sizeof(testParameter_t));
  return UDRV_RETURN_OK;
}

int32_t service_lora_get_tconf(testParameter_t *Param)
{
  memcpy1(Param, &testParam, sizeof(testParameter_t));
  return UDRV_RETURN_OK;
}

int32_t service_lora_ttx(int32_t nb_packet)
{
  int32_t i;

  if ((TestState & TX_TEST_LORA) != TX_TEST_LORA)
  {
    TestState |= TX_TEST_LORA;
    udrv_serial_log_printf("Tx Test\r\n");
    /* Radio initialization */
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;
    Radio.Init(&RadioEvents);
    /*Fill payload with PRBS9 data*/
    Prbs9_generator(payload, testParam.payloadLen);
    LORA_TEST_DEBUG("TX frequency %d", testParam.frequency);
    LORA_TEST_DEBUG("TX power %d", testParam.power);
    LORA_TEST_DEBUG("TX modem %d", testParam.modem);
    LORA_TEST_DEBUG("TX bandwidth %d", testParam.bandwidth);
    LORA_TEST_DEBUG("TX datarate %d", testParam.datarate);
    LORA_TEST_DEBUG("TX coderate %d", testParam.coderate);
    LORA_TEST_DEBUG("TX fixLen %d", testParam.fixLen);
    LORA_TEST_DEBUG("TX payloadLen %d", testParam.payloadLen);
    LORA_TEST_DEBUG("TX preambleLen %d", testParam.preambleLen);
    LORA_TEST_DEBUG("TX FreqHopOn %d", testParam.FreqHopOn);
    LORA_TEST_DEBUG("TX modem %d", testParam.modem);
    LORA_TEST_DEBUG("TX iqInverted %d", testParam.iqInverted);
    LORA_TEST_DEBUG("TX crcOn %d", testParam.crcOn);
    LORA_TEST_DEBUG("TX nb_packet %d", nb_packet);

    /* Launch several times payload: nb times given by user */
    Radio.SetChannel(testParam.frequency);
    Radio.SetTxConfig(testParam.modem, testParam.power, testParam.fdev, testParam.bandwidth, testParam.datarate, testParam.coderate, testParam.preambleLen,
                      testParam.fixLen, testParam.crcOn, testParam.FreqHopOn, testParam.HopPeriod, testParam.iqInverted, testParam.txTimeout);
    packet = nb_packet;
    packet_back = packet;
    TimerInit(&TxTimer, OnTxTimerEvent);
    TimerSetValue(&TxTimer, 500);
    TimerStart(&TxTimer);
    return UDRV_RETURN_OK;
  }
  else
  {
    return -UDRV_INTERNAL_ERR;
  }
}

int32_t service_lora_trx(int32_t nb_packet)
{
  /* init of PER counter */
  if (((TestState & RX_TEST_LORA) != RX_TEST_LORA) && (nb_packet > 0))
  {
    TestState |= RX_TEST_LORA;
    /* Radio initialization */
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;
    Radio.Init(&RadioEvents);

    LORA_TEST_DEBUG("RX frequency %d", testParam.frequency);
    LORA_TEST_DEBUG("RX modem %d", testParam.modem);
    LORA_TEST_DEBUG("RX bandwidth %d", testParam.bandwidth);
    LORA_TEST_DEBUG("RX datarate %d", testParam.datarate);
    LORA_TEST_DEBUG("RX coderate %d", testParam.coderate);
    LORA_TEST_DEBUG("RX fixLen %d", testParam.fixLen);
    LORA_TEST_DEBUG("RX payloadLen %d", testParam.payloadLen);
    LORA_TEST_DEBUG("RX rxContinuous %d", testParam.rxContinuous);
    packet = nb_packet;
    packet_back = packet;
    Radio.SetChannel(testParam.frequency);
    Radio.SetRxConfig(testParam.modem, testParam.bandwidth, testParam.datarate, testParam.coderate, testParam.bandwidthAfc, testParam.preambleLen,
                      testParam.symbTimeout, testParam.fixLen, testParam.payloadLen, testParam.crcOn, testParam.FreqHopOn, testParam.HopPeriod, testParam.iqInverted, testParam.rxContinuous);
    Radio.Rx(RX_TIMEOUT_VALUE);
    return UDRV_RETURN_OK;
  }
  else
  {
    return -UDRV_INTERNAL_ERR;
  }
}

int32_t service_lora_tth(const testParameter_t *param)
{
  if ((TestState & TX_TEST_LORA) != TX_TEST_LORA)
  {
    TestState |= TX_TEST_LORA;
    hop_flag = 1;
    /* Radio initialization */
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;
    Radio.Init(&RadioEvents);

    testParam.freq_start = param->freq_start;
    testParam.freq_stop = param->freq_stop;
    testParam.hp_step = param->hp_step;
    testParam.nb_tx = param->nb_tx;

    LORA_TEST_DEBUG("RX frequency %d", testParam.freq_start);
    LORA_TEST_DEBUG("RX frequency %d", testParam.freq_stop);
    LORA_TEST_DEBUG("RX frequency %d", testParam.hp_step);
    LORA_TEST_DEBUG("RX frequency %d", testParam.nb_tx);

    /*Fill payload with PRBS9 data*/
    Prbs9_generator(payload, testParam.payloadLen);
    freq_start_back = testParam.freq_start;
    packet_back = testParam.nb_tx;
    Radio.SetChannel(testParam.freq_start);
    Radio.SetTxConfig(testParam.modem, testParam.power, testParam.fdev, testParam.bandwidth, testParam.datarate, testParam.coderate, testParam.preambleLen,
                      testParam.fixLen, testParam.crcOn, testParam.FreqHopOn, testParam.HopPeriod, testParam.iqInverted, testParam.txTimeout);
    TimerInit(&TxTimer, OnTxTimerEvent);
    TimerSetValue(&TxTimer, 500);
    TimerStart(&TxTimer);
  }
  else
  {
    return -UDRV_INTERNAL_ERR;
  }

  return UDRV_RETURN_OK;
}

void OnTxDone(void)
{
  udrv_serial_log_printf("OnTxDone\r\n");
  Radio.Sleep();
  LORA_TEST_DEBUG();
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
  LORA_TEST_DEBUG("packet %d", packet);
  Rx_count_RxOk++;
  udrv_serial_log_printf("RssiValue= %d dBm, SnrValue= %d \r\n", rssi, snr);
  Recv_Enent();
}

void OnTxTimeout(void)
{
  LORA_TEST_DEBUG();
}

void OnRxTimeout(void)
{
  LORA_TEST_DEBUG("packet %d", packet);
  Rx_count_RxKo++; 
  Recv_Enent();
}

void OnRxError(void)
{
  LORA_TEST_DEBUG("packet %d", packet);
  Rx_count_RxKo++;
  Recv_Enent();
}

static int32_t Prbs9_generator(uint8_t *payload, uint8_t len)
{
  uint16_t prbs9_val = 2;

  for (int32_t i = 0; i < len * 8; i++)
  {
    /*fill buffer with prbs9 sequence*/
    int32_t newbit = (((prbs9_val >> 8) ^ (prbs9_val >> 4)) & 1);
    prbs9_val = ((prbs9_val << 1) | newbit) & 0x01ff;
    payload[i / 8] |= ((prbs9_val & 0x1) << (i % 8));
  }
  return 0;
}

static void OnTxTimerEvent(void)
{
  LORA_TEST_DEBUG();
  
  if (hop_flag)
  {
    LORA_TEST_DEBUG("nb_tx %d", testParam.nb_tx);

    testParam.nb_tx--;
    Radio.Send(payload, testParam.payloadLen);
    udrv_serial_log_printf("Tx Hop at %d Hz\r\n",testParam.freq_start);
    udrv_serial_log_printf("Tx Test : Packet %d of %d\r\n",( packet_back-testParam.nb_tx),packet_back);
    LORA_TEST_DEBUG("freq %d", testParam.freq_start);

    testParam.freq_start += testParam.hp_step;
    if (testParam.freq_start > testParam.freq_stop)
    {
      testParam.freq_start = freq_start_back;
    }
    Radio.SetChannel(testParam.freq_start);

    if (testParam.nb_tx)
      TimerStart(&TxTimer);
    else
    {
      TestState &= ~TX_TEST_LORA;
      TimerStop(&TxTimer);
      hop_flag = 0;
    }
  }
  else
  {
    LORA_TEST_DEBUG("packet %d", packet);
    packet--;

    Radio.Send(payload, testParam.payloadLen);
    udrv_serial_log_printf("Tx Test : Packet %d of %d\r\n",( packet_back-packet),packet_back);
    if (packet)
      TimerStart(&TxTimer);
    else
    {
      TestState &= ~TX_TEST_LORA;
      TimerStop(&TxTimer);
    }
  }
  
}



static void Recv_Enent(void)
{
  uint32_t PER = 0;
  packet--;
  PER = (100 * (Rx_count_RxKo)) / (Rx_count_RxKo + Rx_count_RxOk);
  udrv_serial_log_printf("Rx %d of %d  >>> PER= %d %%\r\n",(packet_back -packet) ,packet_back, PER);
  if (packet)
  {
    Radio.Rx(RX_TIMEOUT_VALUE);
  }
  else
  {
    TestState &= ~RX_TEST_LORA;
    Rx_count_RxKo=0;
    Rx_count_RxOk=0;
    Radio.Sleep();
  }
}
