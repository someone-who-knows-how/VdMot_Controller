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


#include "VdmTask.h"
#include "VdmNet.h"
#include "stmApp.h"
#include "stm32ota.h"
#include "stm32.h"
#include "ServerServices.h"
#include "VdmConfig.h"
#include <BasicInterruptAbstraction.h>

CVdmTask VdmTask;

CVdmTask::CVdmTask()
{
  taskIdCheckNet=TASKMGR_INVALIDID;
  taskIdMqtt=TASKMGR_INVALIDID;
  taskIdApp=TASKMGR_INVALIDID;
  taskIdStm32Ota=TASKMGR_INVALIDID;
  taskIdServices=TASKMGR_INVALIDID;
  taskIdSetFactoryCfgTimeOut=TASKMGR_INVALIDID;
  taskIdSetFactoryCfgInProgress=TASKMGR_INVALIDID;
  setFactoryCfgState=idle;
}

void CVdmTask::init()
{
    if (taskIdCheckNet==TASKMGR_INVALIDID) {
        taskIdCheckNet = taskManager.scheduleFixedRate(1000, [] {
            VdmNet.checkNet();
        });
    }
}

void CVdmTask::startMqtt(uint32_t interval)
{
    uint32_t thisInterval = 100;
    if (taskIdMqtt==TASKMGR_INVALIDID) {
        if (interval >= 100) thisInterval = interval;
        taskIdMqtt = taskManager.scheduleFixedRate(thisInterval, [] {
            VdmNet.mqttBroker();
        });
    }
}

void CVdmTask::startApp()
{
    if (taskIdStm32Ota!=TASKMGR_INVALIDID) {
        deleteTask (taskIdStm32Ota);
        delay (1000);       // wait to finish task; 
        taskIdStm32Ota=TASKMGR_INVALIDID; 
        Services.restartSystem();
    }
    if (taskIdApp==TASKMGR_INVALIDID) { 
        StmApp.app_setup();
        taskIdApp = taskManager.scheduleFixedRate(100, [] {
                StmApp.app_loop();
        });
    } else {
        taskManager.setTaskEnabled (taskIdApp,true);   
    }
}

void CVdmTask::startStm32Ota(uint8_t command,String thisFileName)
{
    taskManager.setTaskEnabled (taskIdApp,false);
    delay (1000);           // wait to finish task;
    Stm32.STM32ota_setup();
    Stm32.STM32ota_start(command,thisFileName);
    if (taskIdStm32Ota==TASKMGR_INVALIDID) {
        taskIdStm32Ota = taskManager.scheduleFixedRate(30, [] {         // 30 ms good for 115200 baud UART speed and blocksize of 256
            Stm32.STM32ota_loop();
        });
    } else {
        delay (100);         
    }
}


void CVdmTask::startServices()
{
    taskIdRunOnce = taskManager.scheduleOnce(1000, [] {
                Services.runOnce();
    });
    taskIdRunOnceDelayed = taskManager.scheduleOnce(10000, [] {
                Services.runOnceDelayed();
    });
    taskIdServices = taskManager.scheduleFixedRate(60*1000, [] {
        Services.servicesLoop();
    });
}

void CVdmTask::deleteTask (taskid_t taskId)
{
  taskManager.cancelTask (taskId);
}

bool CVdmTask::taskExists (taskid_t taskId)
{
   return (taskManager.getTask(taskId)!=NULL);
}

void CVdmTask::yieldTask (uint16_t ms)
{
    taskManager.yieldForMicros(ms*1000);
}

