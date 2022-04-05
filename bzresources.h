#ifndef BZRESOURCES_H
#define BZRESOURCES_H

#include <QObject>
//#include<QtGlobal>
#include<QStringList>
/**************************************************
 *
 * ProductName:     Teach Pendant v1.0
 * Manufacturer:    BOZHON
 * Author:          JYL
 * Date:            2018.01.24
 *
 * ************************************************/
#define DEBUG_MODE 0
/***********************************************************************/
#define DEFAULT_RC_IP         "192.168.5.228"      //控制器的IP地址
/***********************************************************************/
#define FRAME_HEAD_LENGTH 10    //数据帧头长度
#define ROBOT_ID    0
#define TYPE        0
/***********************************************************************/
#define FUNC_INTER   					0x00/*解释器相关命令*/
#define FUNC_INS     					0x01/*指令命令*/
#define FUNC_SYS     					0x02/*系统命令*/
#define FUNC_TEACH   					0x03/*示教命令*//*要修改该值时需要一并修改Server模块里的值！！！！！！！！*/
/***********************************************************************/
/*						解释器相关命令							*/
/***********************************************************************/
#define INTER_TEST						0x00 /*测试用*/
#define INTER_START 					0x01 /*启动机械手解释器(全速运行)*/
#define INTER_STOP						0x02 /*停止机械手解释器(全速运行)*/
#define INTER_PAUSE 					0x03 /*暂停机械手解释器(全速运行)*/
#define INTER_RESUME					0x04 /*恢复运行机械手解释器(全速运行)*/

#define INTER_DBG_START 				0x11 /*启动机械手解释器 (调试运行)*/
#define INTER_DBG_STEP_INTO 			0x12 /*单步运行 	(调试运行)*/
#define INTER_DBG_STEP_OVER 			0x13 /*逐过程运行	(调试运行)*/
#define INTER_DBG_STEP_OUT				0x14 /*恢复到全速运行	(调试运行)*/
#define INTER_DBG_PRINT_VAR 			0x15 /*打印变量 	(调试运行)*/
#define INTER_DBG_BACK_TRACE			0x16 /*堆栈回溯 	(调试运行)*/
#define INTER_DBG_ADD_BKPOINT			0x17 /*添加断点 	(调试运行)*/
#define INTER_DBG_DEL_BKPOINT			0x18 /*删除断点 	(调试运行)*/
#define INTER_DBG_ADD_WATCH 			0x19 /*添加监控的变量	(调试运行)*/
#define INTER_DBG_PT_VAR				0x21 /*打印变量 	(调试器内部和上位机使用)*/
#define INTER_DBG_PT_LINE				0x22 /*运行时的当前行号(调试器内部和上位机使用)*/
#define DEG_DB_PT_MOVE_LINE 0x23     /*更新运动行号*/
#define INTER_DBG_INS_PT				0x31 /*PRINT打印指令	(调试器内部和上位机使用)*/
#define INTER_DBG_DUMP					0x35 /*内部测试用	(调试器内部和上位机使用)*/
#define INTER_DBG_ROBOT_ERR 			0x40 /*ERROR指令使用	(调试器内部和上位机使用)*/
#define DEG_INTER_BACKGROUND_TASK_START     0x61 /*启动后台任务解释器(全速运行)*/
#define DEG_INTER_BACKGROUND_TASK_STOP      0x62 /*停止后台任务解释器(全速运行)*/
#define DEG_INTER_BACKGROUND_TASK_PAUSE     0x63 /*暂停后台任务解释器(全速运行)*/
#define DEG_INTER_BACKGROUND_TASK_RESUME    0x64 /*恢复运行后台任务解释器(全速运行)*/
#define DEG_INTER_BACKGROUND_TASK_CLEAR     0x65 /*后台任务解释器清除错误(全速运行)*/
/***********************************************************************/
/*						系统控制相关命令						*/
/***********************************************************************/
#define SYS_REBOOT      				0x00 /*重启系统*/
#define SYS_CHANGE_MODE     			0x01 /*当上位机连接下位机成功时，下位机发送此命令给上位机更新上位机界面等必要的初始状态信息*/
#define SYS_DOWNLOAD_SCRIPT 			0x02 /*下载文件到机器人控制器*/
#define SYS_UPLOAD_SCRIPT   			0x03 /*上传文件到上位机*/
#define SYS_CFG_MAIN_FILE   			0x04 /*目前未使用*/
#define SYS_CFG_MOTION      			0x05 /*目前未使用*/
#define SYS_GET_FILE_INFO   			0X06 /*查看下位机文件名*/
#define SYS_GET_TIME_STAMP  			0x07 /*获取下位机文件的修改时间信息*/
#define SYS_DELETE_FILE     			0X08 /*删除下位机文件名*/
#define SYS_CHECKEQUAL_FILE 			0X09 /*获取下位机文件名及其对应的校验码*/
#define SYS_UPDATE_STATUS				0X0A /*更新系统状态*/
#define SYS_CLEAR_FAULT     			0X10 /*清除MOTION所报的错误*/
#define SYS_GET_ECAT_STS    			0x11 /*获取ETHERCAT通信状态*/
#define SYS_GET_MOTION_STS  			0x12 /*获取MOTION状态*/
#define SYS_UPDATE_POSITION             0x13 /*获取位置信息状态*/
#define SYS_GET_FILE_CRC            0x16
#define SYS_SET_FILE_CRC            0x17
#define SYS_CLEAR_SERVO_FAULT 			0x20 /*清除驱动器错误*/
#define SYS_CLEAR_BATTERY_FAULT         0x21
#define SYS_WRITE_PARAMETER				0x30
#define SYS_READ_PARAMETER				0x31
#define SYS_READ_HWVERSION				0x32
#define SYS_GET_AGENT_SERVER    0x35
#define SYS_SET_AGENT_SERVER    0x36
#define SYS_HEARTBEATS                  0X41 /******/
#define SYS_UPDATE_APP					0X51
#define SYS_BACKUP_FILE					0X52
#define SYS_RECOVER_FILE				0X53
#define SYS_UPDATE_ETHERCAT_XML 0X54
#define SYS_GETROBOT_TYPE				0X60
#define SYS_SPEED_PERSENT               0X70
#define SYS_READ_CONVEYOR_PARM                        0X71
#define SYS_WRITE_CONVEYOR_PARM                        0X72
#define SYS_GET_MAC                              0xAA
#define SYS_GET_REGISTER                0xB1/*获得寄存器数据*/
#define SYS_GET_TCP_COM_DATA            0xB2 /*获得网络监控数据*/
#define SYS_STOP_GET_TCP_COM_DATA       0xB3 /*停止获得网络监控数据*/
#define SYS_GET_TCP_COM_DATA_REALTIME   0xB4 /*刷新获得网络监控数据*/
#define SYS_GET_LICENSE                 0xB5 /*获取license*/
#define SYS_GET_IO_STATE                0xC0
#define SYS_GET_AXIS_STATE              0xC1
/***********************************************************************/
/*						示教器相关命令							*/
/***********************************************************************/
#define TEACH_BEGIN      				0x00 /*进入示教模式开始示教*/
#define TEACH_END      	    			0x01 /*停止示教模式停止示教*/
#define TEACH_ACT_MOVEL     			0x02 /*再现功能MOVL*/
#define TEACH_ACT_MOVEJ     			0x03 /*再现功能MOVJ*/
#define TEACH_ACT_MOVEC     			0x04 /*再现功能MOVC*/
#define TEACH_STEP_MOV      			0x05 /*位置模式移动(固定距离运动)*/
#define TEACH_JOG_MOV       			0x06 /*速度模式移动(连续运动)*/
#define TEACH_ACT_JUMP                 0x07
#define TEACH_TASK_OK       			0x08 /*用户进入示教模式时下位机创建示教Task后发送此命令给上位机*/
#define TEACH_REMOTE_BEGIN  0x09 //远程模式
#define TEACH_LOAD_PT_2_LST          	0x12 /*加载一个点位文件里的所有点位数据到单向队列*/
#define TEACH_MODIFY_PT                 0x13
#define TEACH_MODIFY_PT_BY_NAME		 	0x14 /*按名称修改队列里的点位数据*/
#define TEACH_ADD_NEW_PT_2_LIST      	0x16 /*增加一个点位数据到队列*/
#define TEACH_DEL_PT_BY_NAME         	0x17 /*按名称删除队列里的点位数据*/
#define TEACH_DEL_PT_BY_ID           	0x18 /*按索引删除队列里的点位数据*/
#define TEACH_SAVE_ALL_PT_2_FILE     	0x19 /*保存队列里的所有点位到文件里*/
#define TEACH_ADD_CUR_PT_2_LIST      	0x1A /*保存当前位置到队列里上位机发送名称下位机获取位置值合并为点位数据并保存到点位*/
#define TEACH_UPDATE_MORNITOR_DATA   	0x1B /*下位机更新监控数据到上位机*/
#define TEACH_SET_COOR               	0x1C /**/
#define TEACH_SEL_COOR_MODE             0x1D /**/
#define TEACH_SERVO_ON   	 			0X20 /*伺服上电*/
#define TEACH_POWER_HIGH 	 			0X22 /*高功率*/
#define TEACH_BREAK_ON    	 			0X25 /*刹车打开(继电器得电)即刹车张开*/
#define TEACH_GET_ENCODER    			0X2A /*获取编码器值*/
#define TEACH_SET_OUT        			0X2B /*设置I/O 输出值*/
#define TEACH_HEARTBEATS     			0X2C /*要修改该值时需要一并修改Server模块里的值！！！！！！！！*/
#define TEACH_ZX_FINISH      			0X2D /*点位在现运动完成*/
#define TEACH_LOAD_TOOL_2_LST          	0x32 /*加载一个工具坐标系文件里的所有工具坐标系数据到单向队列*/
#define TEACH_REMOVE_ALL_TOOL_FROM_LST  0x33
#define TEACH_MODIFY_TOOL_BY_NAME		0x34 /*按名称修改队列里的工具坐标系数据*/
#define TEACH_MODIFY_TOOL_BY_ID        	0x35 /*按索引修改队列里的工具坐标系数据*/
#define TEACH_ADD_NEW_TOOL_2_LIST      	0x36 /*增加一个工具坐标系数据到队列*/
#define TEACH_DEL_TOOL_BY_NAME         	0x37 /*按名称删除队列里的工具坐标系数据*/
#define TEACH_DEL_TOOL_BY_ID           	0x38 /*按索引删除队列里的工具坐标系数据*/
#define TEACH_SAVE_ALL_TOOL_2_FILE     	0x39 /*保存队列里的所有工具坐标系到文件里*/
#define TEACH_ADD_CUR_TOOL_2_LIST      	0x3A /*保存当前位置到工具坐标系队列里，上位机发送名称+下位机获取位置值，合并为工具坐标系数据并保存到工具坐标系*/
#define TEACH_LOAD_USER_2_LST          	0x42 /*加载一个用户坐标系文件里的所有用户坐标系数据到单向队列*/
#define TEACH_REMOVE_ALL_USER_FROM_LST  0x43
#define TEACH_MODIFY_USER_BY_NAME		0x44 /*按名称修改队列里的用户坐标系数据*/
#define TEACH_MODIFY_USER_BY_ID        	0x45 /*按索引修改队列里的用户坐标系数据*/
#define TEACH_ADD_NEW_USER_2_LIST      	0x46 /*增加一个用户坐标系数据到队列*/
#define TEACH_DEL_USER_BY_NAME         	0x47 /*按名称删除队列里的用户坐标系数据*/
#define TEACH_DEL_USER_BY_ID           	0x48 /*按索引删除队列里的用户坐标系数据*/
#define TEACH_SAVE_ALL_USER_2_FILE     	0x49 /*保存队列里的所有用户坐标系到文件里*/
#define TEACH_ADD_CUR_USER_2_LIST      	0x4A /*保存当前位置到用户坐标系队列里，上位机发送名称+下位机获取位置值，合并为用户坐标系数据并保存到用户坐标系*/
#define TEACH_SAVE_CALI_DATA	      	0x52 /*保存当前位置到标定用的缓冲区*/
#define TEACH_CALCULATE_CALI_DATA	    0x53 /*计算标定结果*/
#define TEACH_SAVE_CALI_DATA_NEW              0x54 /*保存当前位置到标定用的缓冲区*/
#define TEACH_SAVE_CALI_DATA_NEW_4POINT 0x55 /*保存4点到标定用的缓冲区*/
#define TEACH_CALCULATE_CALI_DATA_NEW        0x56 /*计算标定结果*/
#define TEACH_SAVE_3_PT_CALI_DATA              0x57 /*保存当前位置到标定用的缓冲区*/
#define TEACH_CALCULATE_3_PT_CALI_DATA        0x58 /*计算标定结果*/
#define TEACH_SET_CONVEYOR_PARM			0x65 /*设置皮带跟踪参数*/
#define TEACH_SET_CONVEYOR_ENC_FACTOR	0x66 /*设置皮带跟踪编码器当量*/
#define TEACH_GET_CONVEYOR_ENC_FACTOR 0x67
#define TEACH_GET_ADDI_ENCODER  0x70 /*获取外部轴编码器*/
#define TEACH_GO_HOME                                        0x80 /*命令行回原点*/
#define TEACH_EXAXIS_GO_HOME 0x82
#define TEACH_GSCARA_GO_HOME                        0x84 /*gscara回原点*/
#define SYS_UPDATE_TEACH_BOX_PARM  0X95
#define SYS_UPDATE_TEACH_TOOL_BOX_PARM 0x96
#define TEACH_SAVE_HOME_CALI_DATA 	0X90
#define TEACH_CALCULATE_HOME_CALI_DATA 0X91
#define TEACH_GO_REF_PULSE  0x9A
#define TEACH_TM_TURN_RESET 0x9B
#define TEACH_CONNECT_CAMERA_SERVER          0xA0 /*连接相机*/
#define TEACH_DISCONNECT_CAMERA_SERVER       0xA1/*断开相机*/
#define TEACH_GET_CAMERA_CONNECT_STATE       0xA2/*连接状态*/
#define TEACH_GET_CAMERA_TOOL_CURRENT_POSITION    0xA3/*获取相机工具坐标系*/
#define TEACH_CAL_CAMERA_TOOL             0xA4/*标定相机工具*/
#define TEACH_GET_CAMERA_PIX_POSITION     0xA5/*保存工具*/
#define TEACH_CALI_VISION_PARAMETER           0xA6/*连接状态*/
#define TEACH_SAVE_CAMERA_POSITION     0xA7/*保存机器人位置*/
#define TEACH_SELECT_ROBOT_ID                                0xA8/*选择机器人ID*/
#define TEACH_READ_ROBOT_ID                                        0xA9/*读取机器人ID*/
#define TEACH_SET_CAMERA_PIX_POSITION 0xAA
#define TEACH_AXIS_MOVE                                        0xB7
#define TEACH_AXIS_CLEAR_ERROR                        0xB8
#define TEACH_AXIS_SERVO_ON  0xB9

#endif // BZRESOURCES_H
