/**
 * License Agreement
 * Copyright (c) 2017 Analog Devices Inc.All rights reserved.
 * This source code is intended for the recipient only under the guidelines of the non-disclosure agreement with Analog Devices Inc.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * Warning: Do not Attempt to Refractor/Modify methods/variables of this file
 */

//class watchCallback;
#import "gen4sdkWrapper.h"
#import "gen4sdkBLE.h"
uint8_t rr_pattern;
uint8_t gg_pattern;
int16_t x_value,y_value,z_value;   //////adxl X Y Z values
NSString  *ECGleadsoff;   //!< ECG lead detection
uint8_t  ECGHRVALUE;         //!< ECG algorithm heart rate
uint16_t  ecg_data_Value;   //!< ECG data value
NSString  *ECGTYpe;   //!< ECG Type
uint8_t setrr_pattern;
uint8_t setgg_pattern;
