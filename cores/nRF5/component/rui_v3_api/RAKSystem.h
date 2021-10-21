#ifndef __SYSTEM_H__
#define __STSTEM_H__

#include <string>
#include <sstream>

#include "sleep.h"
#include "atcmd.h"
#include "udrv_errno.h"
#include "udrv_system.h"
#include "udrv_serial.h"
#include "udrv_delay.h"
#include "service_mode_cli.h"
#include "service_battery.h"
#include "service_nvm.h"
#include "service_fs.h"

using namespace std;

class RAKSystem {
  public:
    RAKSystem();
    class sleep sleep;

    class firmwareVersion {
      private:
        string ver;

      public:
        firmwareVersion();

	/**@par		Description
	 * 		This api allow user to get the firmware version
	 * @ingroup	Firmware_Version
	 * @par		Syntax
	 * 		api.system.firmwreVersion.get()
	 * @return	firmware version(Type: string)
	 * @par		Example
	 * @verbatim	
	   void setup()
	   {
	     Serial.begin(115200);
	   }

	   void loop()
	   {
             Serial.printf("Firmware Version: %s\r\n", api.system.firmwareVersion.get().c_str());
	     delay(1000);
	   }
	   @endverbatim
	 */
        const string get();
    };

    class cliVersion {
      private:
        string ver;

      public:
        cliVersion();

	/**@par		Description
	 *		This api allow user to get the cli version
	 * @ingroup	Cli_Version
	 * @par		Syntax
	 *		api.system.cliVersion.get()
	 * @return	cli version(Type: string)
	 * @par         Example
         * @verbatim
           void setup()
           {
             Serial.begin(115200);
           }

           void loop()
           {
             Serial.printf("AT Command Version: %s\r\n", api.system.cliVersion.get().c_str());
             delay(1000);
           }
           @endverbatim

	 */
        const string get();
    };

    class apiVersion {
      private:
        string ver;

      public:
        apiVersion();

	/**@par		Description
	 *		This api allow user to get the api version
	 * @ingroup	Api_Version
	 * @par		Syntax
	 * 		api.system.apiVersion.get()
	 * @return	api version(Type: string)
	 * @par         Example
         * @verbatim
           void setup()
           {
             Serial.begin(115200);
           }

           void loop()
           {
             Serial.printf("RUI API Version: %s\r\n", api.system.apiVersion.get().c_str());
             delay(1000);
           }
           @endverbatim

	 */
        const string get();
    };

    class modelId {
      private:
        string id;

      public:
        modelId();

	/**@par		Description
	 *		This api allow user to get the mode ID
	 * @ingroup	Model_ID
	 * @par		Syntax
	 * 		api.system.modelId.get()
	 * @return	model ID(Type: string)
	 * @par         Example
         * @verbatim
           void setup()
           {
             Serial.begin(115200);
           }

           void loop()
           {
             Serial.printf("Model ID: %s\r\n", api.system.modelId.get().c_str());
             delay(1000);
           }
           @endverbatim

	 */
        const string get();
    };

    class chipId {
      private:
        string id;

      public:
        chipId();

	/**@par		Description
	 *		This api allow user to get the chip ID
	 * @ingroup	Chip_ID
	 * @par		Syntax
	 *		api.system.chipId.get()
	 * @return	chip ID(Type: string)
	 * @par         Example
         * @verbatim
           void setup()
           {
             Serial.begin(115200);
           }

           void loop()
           {
             Serial.printf("Hardware ID: %s\r\n", api.system.chipId.get().c_str());
             delay(1000);
           }
           @endverbatim

	 */
        const string get();
    };
    
    firmwareVersion firmwareVersion;
    cliVersion cliVersion;
    apiVersion apiVersion;
    modelId modelId;
    chipId chipId;

    /**@par	Description
     *
     * @ingroup	System_Misc
     * @par	Syntax
     * 		api.system.reboot()
     * @return	void
     * @par	Example
     * @verbatim
       int loopCount == 0;

       void setup()
       {
       }

       void loop()
       {
         loopCount++;

         if(loopCount == 60)
           api.system.reboot();  // Reboot after 60 seconds 

         delay(1000);
       }
       @endverbatim 
     */
    void reboot();

    /**@par	Description
     *
     * @ingroup	System_Misc
     * @par	Syntax
     * 		api.system.restoreDefault()
     * @return	void
     * @par	Example
     * @verbatim
       void setup()
       {
         api.system.restoreDefault();
       }

       void loop()
       {
       }
       @endverbatim 
     */
    void restoreDefault();

    class pword {
      public:
	/**@par		Description
	 *		This api allow user to set a 1~8 digits password to lock the default serial
	 * @ingroup	System_Serial
	 * @par		Syntax
	 * 		api.system.pword.set(passwd_Str)\n
	 * 		api.system.pword.set(passwd_Char, len)
	 * @param	passwd_Str	the password to lock the Default Serial(Type: string)
	 * @param	passwd_Char	the password to lock the Default Serial(Type: cahr *)
	 * @param	len		the length of passwod
	 * @return	true for successfully set a password, false for set a password fail
	 * @par		Example
	 * @verbatim
	   int loopCount = 0;

	   void setup()
	   {
	     string password = "12345678";
	     api.system.pword.set(password); // set the password to 12345678
	     api.system.pword.lock();        // lock the default port 
	   }

	   void loop()
	   {
	     loopCount++; 

             if (loopCount == 60)
               api.system.pword.unlock();   // unlock the default port after 60 seconds

	     delay(1000);
	   }
	   @endverbatim
	 */
        bool set(char *passwd, uint32_t len);
        bool set(string passwd);

	/**@par		Description
	 *		This api allow user to lock the default serial with the pass set in api.system.pword.set()	
	 * @note	If you never set a password successfully, the default password will be 00000000
	 * @ingroup	System_Serial
	 * @par		Syntax
	 * 		api.system.pword.lock()
	 * @return	void
	 * @par         Example
         * @verbatim
           int loopCount = 0;

           void setup()
           {
             string password = "12345678";
             api.system.pword.set(password); // set the password to 12345678
             api.system.pword.lock();        // lock the default port
           }

           void loop()
           {
             loopCount++;

             if (loopCount == 60)
               api.system.pword.unlock();   // unlock the default port after 60 seconds

             delay(1000);
           }
           @endverbatim

	 */
        void lock(void);

	/**@par		Description
	 *		This api allow user to unlock the default serial without password when it'locked
	 * @ingroup	System_Serial
	 * @par		Syntax
	 * 		api.system.pword.unlock()
	 * @return	void
	 * @par         Example
         * @verbatim
           int loopCount = 0;

           void setup()
           {
             string password = "12345678";
             api.system.pword.set(password); // set the password to 12345678
             api.system.pword.lock();        // lock the default port
           }

           void loop()
           {
             loopCount++;

             if (loopCount == 60)
               api.system.pword.unlock();   // unlock the default port after 60 seconds

             delay(1000);
           }
           @endverbatim

	 */
        void unlock(void);
    };

    class bat {
      public:
	/**@par		Description
	 *		
	 * @ingroup	System_Battery
	 * @par		Syntax
	 * 		api.system.bat.get()
	 * @return
	 * @par         Example
         * @verbatim
           void setup()
           {
             Serial.begin(115200);
           }

           void loop()
           {
             Serial.printf("Battery Level: %f\r\n", api.system.bat.get());
             delay(1000);
           }
           @endverbatim

	 */
        float get(void);
    };

    /**@ingroup		System_Misc
     */
    class atMode {
      public:
	/**@par		Description
	 *
	 * @par		Syntax
	 * 		api.system.atMode.add(cmd, usage, title, handle)
	 * @param	cmd	
	 * @param	usage
	 * @param	title
	 * @param	handle
	 *
	 * @return
	 * @par		Example
	 * @verbatim
      uint32_t led_status;
      
      int led_handle(SERIAL_PORT port, char *cmd, stParam *param) {
          if (param->argc == 1 && !strcmp(param->argv[0], "?")) {
              Serial.print(cmd);
              Serial.print("=");
              Serial.println(led_status?"HIGH":"LOW");
          } else if (param->argc == 1) {
              for (int i = 0 ; i < strlen(param->argv[0]) ; i++) {
                  if (!isdigit(*(param->argv[0]+i))) {
                      return AT_PARAM_ERROR;
                  }
              }
      
              led_status = strtoul(param->argv[0], NULL, 10);
              if (led_status != 0 && led_status != 1) {
                  return AT_PARAM_ERROR;
              }
      
              pinMode(GREEN_LED, OUTPUT);
              pinMode(BLUE_LED, OUTPUT);
              digitalWrite(GREEN_LED, (led_status == 1)?HIGH:LOW);
              digitalWrite(BLUE_LED, (led_status == 1)?HIGH:LOW);
          } else {
              return AT_PARAM_ERROR;
          }
      
          return AT_OK;
      }
      
      void setup()
      {
          api.system.atMode.add("LED", "This controls both green and blue LEDs.", "LED", led_handle);
      }
      
      void loop()
      {
      }
	   @endverbatim
	 */
        bool add(char *cmd, char *usage, char *title, PF_handle handle);
    };

    class fs {
      public:

	/**@par         Description
	 *              Get the error code of a file operation
	 *
	 * @ingroup     File_System
	 *
	 * @par		Syntax
	 *              api.system.fs.ferrno(file)
	 *
	 * @param       file     the file handle
	 *
	 * @return      int32_t
	 *
	 */
        int32_t         ferrno(SERVICE_FS_FILE file);

	/**@par		Description
	 *              Get the error code of a directory operation
	 *
	 * @ingroup	File_System
	 *
	 * @par		Syntax
	 *              api.system.fs.derrno(dir)
	 *
	 * @param       dir      the directory handle
	 *
	 * @return      int32_t
	 *
	 */
        int32_t         derrno(SERVICE_FS_DIR dir);

	/**@par		Description
	 *              Initialize the file system
	 *
	 * @ingroup	File_System
	 * @par		Syntax
	 *              api.system.fs.init()
	 *
	 * @return      SERVICE_FS
	 */
        SERVICE_FS             init(void);

	/**@par		Description
	 *              Reset the file system
	 *
	 * @ingroup	File_System
	 * @par		Syntax
	 *              api.system.fs.reset()
	 *
	 * @return      SERVICE_FS
	 */
        SERVICE_FS             reset(void);

	/**@par		Description
	 *              List all the files
	 *
	 * @ingroup	File_System
	 * @par		Syntax
	 *              api.system.fs.list(fs, d, entry)
	 *
         * @param	fs	the file system handle
         * @param	d	the directory handle opened by api.system.fs.opendir()
         * @param	entry	the iterator to store the current entry
         *
	 * @return      int32_t
         * @par	Example
         * @verbatim
         void setup() {
             Serial.begin(115200);
         }

         void loop() {
             int32_t ret;
             SERVICE_FS_DIRENT entry;
             SERVICE_FS_DIR dir;

             if ((ret = api.system.fs.opendir(fs, "/", &dir)) != SERVICE_FS_OK) {
                 Serial.printf("open dir error! %d\r\n", ret);
                 return;
             }

             while (api.system.fs.list(fs, &dir, &entry) != SERVICE_FS_OK) {
                 Serial.printf("%s size:%u\r\n", entry.dirent.name, entry.dirent.size);
             }

             if ((ret = api.system.fs.closedir(fs, &dir)) != SERVICE_FS_OK) {
                 Serial.printf("close dir error! %d\r\n", ret);
                 return;
             }

             Serial.printf("total %lu, used %lu\r\n", entry.total, entry.used);
             delay(5000);
         }
         @endverbatim
         */
        int32_t                list(SERVICE_FS fs, SERVICE_FS_DIR *d, SERVICE_FS_DIRENT *entry);

	/**@par		Description
	 *              Open a file by file path
	 *
	 * @ingroup	File_System
	 * @par		Syntax
	 *              api.system.fs.open(fs, path, flags, mode)
	 *
         * @param	fs	the file system handle
         * @param	path	the file path to be opened
         * @param	flags	open file flags
         * @param	mode	open file mode
	 *
	 * @return      the file handle
	 */
        SERVICE_FS_FILE        open(SERVICE_FS fs, const char* path, uint32_t flags, uint32_t mode);

	/**@par		Description
	 *              Read the content of a file
	 *
	 * @ingroup	File_System
	 * @par		Syntax
	 *              api.system.fs.read(fs, file, buf, len)
	 *              
         * @param	fs	the file system handle
         * @param	file	the file handle
         * @param	buf	the read data buffer
         * @param	len	the number of bytes to be read
         *
	 * @return      int32_t
	 *
	 */
        int32_t                read(SERVICE_FS fs, SERVICE_FS_FILE file, void* buf, uint32_t len);

	/**@par		Description
	 *              Write the content of a file
	 *
	 * @ingroup	File_System
	 * @par		Syntax
	 *              api.system.fs.write(fs, file, buf, len)
	 *              
         * @param	fs	the file system handle
         * @param	file	the file handle
         * @param	buf	the write data buffer
         * @param	len	the number of bytes to be written
         *
	 * @return      int32_t
	 *
	 */
        int32_t                write(SERVICE_FS fs, SERVICE_FS_FILE file, void* buf, uint32_t len);

	/**@par		Description
	 *              Move read/write pointer, Expand size
	 *
	 * @ingroup	File_System
	 * @par		Syntax
	 *              api.system.fs.lseek(fs, file, offs, whence)
	 *
         * @param	fs	the file system handle
         * @param	file	the file handle
         * @param	offs	how much/where to move the offset
         * @param	whence	if SPIFFS_SEEK_SET, the file offset shall be set to offset bytes\nif SPIFFS_SEEK_CUR, the file offset shall be set to its current location plus offset\nif SPIFFS_SEEK_END, the file offset shall be set to the size of the file plus offse, which should be negative
	 *
	 * @return      int32_t
	 */
        int32_t                lseek(SERVICE_FS fs, SERVICE_FS_FILE file, int32_t offs, uint32_t whence);

	/**@par		Description
	 *              Remove a file by file path
	 *
	 * @ingroup	File_System
	 * @par		Syntax
	 *              api.system.fs.remove(fs, path)
	 *
         * @param	fs	the file system handle
         * @param	path	the file path to be removed
	 *
	 * @return      int32_t
	 */
        int32_t                remove(SERVICE_FS fs, const char* path);

	/**@par		Description
	 *              Remove a file by file handle
	 *
	 * @ingroup	File_System
	 * @par		Syntax
	 *              api.system.fs.fremove(fs, file)
	 *              
         * @param	fs	the file system handle
         * @param	file	the file handle
	 *
	 * @return      int32_t
	 */
        int32_t                fremove(SERVICE_FS fs, SERVICE_FS_FILE file);

	/**@par		Description
	 *              Gets file status by path
	 *
	 * @ingroup	File_System
	 * @par		Syntax
	 *              api.system.fs.stat(fs, path, stat)
	 *
         * @param	fs	the file system handle
         * @param	path	the file path to stat
         * @param	stat	the stat structure to pupulate
	 *
	 * @return      int32_t
	 */
        int32_t                stat(SERVICE_FS fs, const char* path, SERVICE_FS_FILE_STAT* stat);

	/**@par		Description
	 *              Gets file status by file handle
	 *
	 * @ingroup	File_System
	 * @par		Syntax
	 *              api.system.fs.fstat(fs, file, stat)
	 *
         * @param	fs	the file system handle
         * @param	file	the file handle to stat
         * @param	stat	the stat structure to pupulate
	 *
	 * @return      int32_t
	 */
        int32_t                fstat(SERVICE_FS fs, SERVICE_FS_FILE file, SERVICE_FS_FILE_STAT* stat);

	/**@par		Description
	 *              Flushes all pending write operations from cache for given file
	 *
	 * @ingroup	File_System
	 * @par		Syntax
	 *              api.system.fs.flush(fs, file)
	 *
         * @param	fs	the file system handle
         * @param	file	the file handle to flush
	 *
	 * @return      int32_t
	 */
        int32_t                flush(SERVICE_FS fs, SERVICE_FS_FILE file);

	/**@par		Description
	 *              Closes a file handle
	 *
	 * @ingroup	File_System
	 * @par		Syntax
	 *              api.system.fs.close(fs, file)
	 *
         * @param	fs	the file system handle
         * @param	file	the file handle to close
	 *
	 * @return      int32_t
	 */
        int32_t                close(SERVICE_FS fs, SERVICE_FS_FILE file);

	/**@par		Description
	 *              Renames a file
	 *
	 * @ingroup	File_System
	 * @par		Syntax
	 *              api.system.fs.rename(fs, old_name, new_name)
	 *
         * @param	fs		the file system handle
         * @param	old_name	the path of file to rename
         * @param	new_name	the new path of file
	 *
	 * @return      int32_t
	 */
        int32_t                rename(SERVICE_FS fs, const char* old_name, const char* new_name);

	/**@par		Description
	 *              Opens a directory stream corresponding to the given name.
	 *
	 * @ingroup	File_System
	 * @par		Syntax
	 *              api.system.fs.opendir(fs, name, d)
	 *
         * @param	fs		the file system handle
         * @param	name		the name of the directory
         * @param	d		pointer the directory stream to be populated
	 *
	 * @return      int32_t
	 */
        int32_t                opendir(SERVICE_FS fs, const char* name, SERVICE_FS_DIR* d);

	/**@par		Description
	 *              Closes a directory stream
	 *
	 * @ingroup	File_System
	 * @par		Syntax
	 *              api.system.fs.closedir(fs, d)
	 *
         * @param	fs		the file system handle
         * @param	d		the directory stream to close
	 *
	 * @return      int32_t
	 */
        int32_t                closedir(SERVICE_FS fs, SERVICE_FS_DIR* d);

	/**@par		Description
	 *              Reads a directory into given iterator.
	 *
	 * @ingroup	File_System
	 * @par		Syntax
	 *              api.system.fs.readdir(fs, d, e)
	 *
         * @param	fs		the file system handle
         * @param	d		pointer to the directory stream
         * @param	e		the iterator structure to be populated
	 *
	 * @return      null if error or end of stream, else an entry is returned
	 */
        SERVICE_FS_DIRENT*     readdir(SERVICE_FS fs, SERVICE_FS_DIR* d, SERVICE_FS_DIRENT* e);

	/**@par		Description
	 *              Check if EOF reached.
	 *
	 * @ingroup	File_System
	 * @par		Syntax
	 *              api.system.fs.feof(fs, file)
	 *
         * @param	fs		the file system handle
         * @param	file		the file handle
	 *
	 * @return      int32_t
	 */
        int32_t                feof(SERVICE_FS fs, SERVICE_FS_FILE file);

	/**@par		Description
	 *              Get position in file.
	 *
	 * @ingroup	File_System
	 * @par		Syntax
	 *              api.system.fs.ftell(fs, file)
	 *
         * @param	fs		the file system handle
         * @param	file		the file handle
	 *
	 * @return      int32_t
	 */
        int32_t                ftell(SERVICE_FS fs, SERVICE_FS_FILE file);
    };

    pword pword;
    bat bat;
    atMode atMode;
    fs fs;


};

/**@example	System_FS/src/app.cpp
 */

#endif
