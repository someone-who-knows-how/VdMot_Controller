/**HEADER*******************************************************************
  project : VdMot Controller

  author : SurfGargano, Lenti84

  Comments:

  Version :

  Modifcations :


***************************************************************************
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE DEVELOPER OR ANY CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License.
  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  Copyright (C) 2021 Lenti84  https://github.com/Lenti84/VdMot_Controller

*END************************************************************************/



#pragma once

#include <Arduino.h>
#include <SPIFFS.h>
#include <FS.h>
#include <CRC32.h>


#define     STM32OTA_START          0x12
#define     STM32OTA_STARTBLANK     0x45
#define     STM32OTA_BLOCKSIZE      256

typedef struct {
  File fsfile;
  unsigned long size;
  unsigned int blockcnt;
  unsigned int lastbytes;
  uint32_t crc;
} flashfile;


enum otaUpdateStatus  {updNotStarted,updStarted,updInProgress,updFinished,updError};


enum ota_state {STM32OTA_IDLE, STM32OTA_PREPARE, STM32OTA_SENDSIGN, \
                STM32OTA_INITSTM, STM32OTA_ERASE_START, STM32OTA_ERASE_FIN, STM32OTA_PREPAREFILE, \
                STM32OTA_VERIFY, STM32OTA_VERIFYREAD, STM32OTA_GETID, STM32OTA_FLASH, STM32OTA_STARTOVER, \
                STM32OTA_ERROR };


class CStm32 
{
public:
  CStm32();
  void STM32ota_setup();
  void STM32ota_begin();
  void STM32ota_start(uint8_t command, String thisFileName);
  void STM32ota_loop();
  void FlashMode();
  void RunMode();
  void ResetSTM32(bool useTask = false);

  volatile uint8_t stmUpdPercent;
  volatile otaUpdateStatus stmUpdateStatus;
  File dir;
  File file;

  flashfile myflashfile;
  String updateFileName;
private:
  int PrepareFile(String FileName);
  int FlashBytes(int Block, int Bytes);
  unsigned char stm32StartRead(unsigned long rdaddress, unsigned int rdlen);
  int stm32ota_command;
  ota_state stm32ota_state;
  
  int timeout;
  int count;
  unsigned char buffer[STM32OTA_BLOCKSIZE];
  unsigned char id;
  uint8_t skipsigning;
  int blockcounter;
  uint32_t timer;

  String tempstr;
  uint32_t  tempcrc;
  CRC32 crc;

};

extern CStm32 Stm32;
