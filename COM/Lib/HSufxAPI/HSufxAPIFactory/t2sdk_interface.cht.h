#include <stdio.h>
#include <stdlib.h>

#ifndef _T2SDK_INTERFACE_H
# define _T2SDK_INTERFACE_H

#ifdef _WIN32
	#if !defined( FUNCTION_CALL_MODE )
	#define FUNCTION_CALL_MODE		__stdcall
	#endif
#else
	#define FUNCTION_CALL_MODE
#endif

#ifndef HS_IKNOWN_SDK
#define HS_IKNOWN_SDK
struct IKnown
{
    virtual unsigned long  FUNCTION_CALL_MODE QueryInterface(const char *iid, IKnown **ppv) = 0;

    virtual unsigned long  FUNCTION_CALL_MODE AddRef() = 0;

    virtual unsigned long  FUNCTION_CALL_MODE Release() =  0;
};
#endif

#include <string.h>

#ifdef _WIN32
typedef unsigned int		uint32;
#else
#include <sys/types.h>
#if defined(__linux__)
#include <stdint.h>
#endif
typedef uint32_t			uint32;
#endif

///ESB組名長度，名字為可見字符，不能包含實例分隔符、空格、分號;
#define IDENTITY_NAME_LENGTH	32
///實例編號最大占位長度
#define ID_LENGTH               4 
///節點名全長,定義時使用char sName[ID_STR_LEN+1]
#define ID_STR_LEN		       (IDENTITY_NAME_LENGTH + ID_LENGTH + 1)


//	插件接口名的最大長度,定義時使用char sName[PLUGINID_LENGTH+1]
#define PLUGINID_LENGTH	256
//	插件實例名的最大長度,定義時使用char sName[PLUGIN_NAME_LENGTH+1]
#define PLUGIN_NAME_LENGTH	(PLUGINID_LENGTH+ID_LENGTH+1)
//	進程名最大長度.定義時使用char sName[SVR_NAME_LENGTH+1]
#define SVR_NAME_LENGTH	256
//	進程實例名最大長度.定義時使用char sName[PLUGINID_NAME_LENGTH+1]
#define SVRINSTANCE_NAME_LENGTH	(SVR_NAME_LENGTH+ID_LENGTH+1)
//業務消息類型


//請求
#define REQUEST_PACKET 0 
//應答
#define ANSWER_PACKET  1 
//20110302 xuxp 增加路由信息的結構體定義
typedef struct tagRouteInfo
{
	char ospfName[ID_STR_LEN+1];
	char nbrName[ID_STR_LEN+1];
	char svrName[SVRINSTANCE_NAME_LENGTH+1];
	char pluginID[PLUGIN_NAME_LENGTH+1];
	int connectID;
	int memberNO;

	tagRouteInfo()
	{
		memset(this,0,sizeof(tagRouteInfo));
	}
}Route_Info;


//20101228 xuxp 為了發送和返回訂閱推送信息而增加的結構體的定義
typedef struct tagRequestData
{	
	int sequeceNo;
	int issueType;
	void* lpKeyInfo;
	int keyInfoLen;
	void* lpFileHead;
	int fileHeadLen;
	int packetType;//20100111 xuxp 新加的包類型
	Route_Info routeInfo;//20110302 xuxp 請求裡面增加路由信息
	int iSubSystemNo;//20130508 xuxp 參數中增加子系統號傳入
}REQ_DATA, *LPREQ_DATA;
typedef struct tagRespondData
{
	int functionID;
	int returnCode;
	int errorNo;
	char* errorInfo;
	int issueType;
	void* lpKeyInfo;
	int keyInfoLen;
	Route_Info sendInfo;//20110302 xuxp 應答裡面增加發送者信息
}RET_DATA, *LPRET_DATA;


///解包器接口
struct IF2ResultSet : public IKnown
{

	///取字段數
    /**@return 返回字段數.
      */
    virtual int FUNCTION_CALL_MODE GetColCount()=0;

    ///取字段名
    /** @param column:字段序號(以0為基數)
      * @return 返回字段名  下標越界 返回NULL
      */
    virtual const char * FUNCTION_CALL_MODE GetColName(int column)=0;

    //取字段數據類型
    /** @param column:字段序號(以0為基數)
      * @return:見上面常量定義;下標越界 返回-1
      */
    virtual char FUNCTION_CALL_MODE GetColType(int column)=0;


   ///取數字型字段小數位數
   /** @param column:字段序號(以0為基數)
     * @return int 返回字段數據小數位數 下標越界 返回-1
     */
   virtual int FUNCTION_CALL_MODE GetColScale(int column)=0;
  
   //取字段允許存放數據的最大寬度.
   /** @param column:字段序號(以0為基數)
     * @return int 返回字段寬度 下標越界 返回-1
     */
   virtual int FUNCTION_CALL_MODE GetColWidth(int column) = 0;

   ///取字段名對應的字段序號
   /**@param columnName: 字段名
     *@return 返回字段序號. 不存在返回-1
     */
   virtual int  FUNCTION_CALL_MODE FindColIndex(const char * columnName)=0;

    //
    //按字段序號(以0為基數)，取字段值(字符串)
    /**@param column:字段序號(以0為基數)
      *@return 字符串型字段值,下標越界返回NULL
      */
    virtual const char *  FUNCTION_CALL_MODE GetStrByIndex(int column)=0;

    //按字段名，取字段值(字符串)
    /**@param columnName: 字段名
      *@return 字符串型字段值,不存在返回NULL
      */
    virtual const char *  FUNCTION_CALL_MODE GetStr(const char * columnName)=0;

    //按字段序號(以0為基數)，取字段值
    /**@param column:字段序號(以0為基數)
      *@return 字符型字段值,下標越界返回'\0'.
      */
     virtual char  FUNCTION_CALL_MODE  GetCharByIndex(int column)=0;

    //按字段名，取字段值
    /**@param columnName: 字段名
     *@return 字符型字段值,不存在返回'\0'
     */
    virtual char   FUNCTION_CALL_MODE GetChar(const char * columnName)=0;

    //按字段序號，取字段值
    /**@param column:字段序號(以0為基數)
      *@return double型字段值,下標越界返回0
      */
    virtual double  FUNCTION_CALL_MODE GetDoubleByIndex( int column)=0;
    
	///按字段名，取字段值
    /**@param columnName:字段名
      *@return double型字段值,不存在返回0
      */
    virtual double  FUNCTION_CALL_MODE GetDouble(const char * columnName)=0;

    ///按字段序號，取字段值
    /**@param column:字段序號(以0為基數)
      *@return int型字段值,下標越界返回0
      */
    virtual int  FUNCTION_CALL_MODE GetIntByIndex(int column)=0;

    ///按字段名，取字段值
    /**@param columnName:字段名
      *@return int型字段值,不存在返回0
      */
    virtual int FUNCTION_CALL_MODE GetInt(const char * columnName)=0;

	///按字段序號獲得字段值,二進制數據
	/**@param column: 字段序號(以0為基數)
	*@param   lpRawLen: [out]數據長度
	*@return : 數據首地址
	*/
	virtual void *  FUNCTION_CALL_MODE GetRawByIndex(int column,int * lpRawLen) = 0;

	///按字段名，取字段值
	/**@param columnName:字段名
	*@param   lpRawLen: [out]數據長度
	*@return : 數據首地址
	*/
	virtual void *  FUNCTION_CALL_MODE GetRaw(const char * columnName,int * lpRawLen) = 0;


	///最後一次取的字段值是否為NULL
   /**@return 0 是， 1不是
   */
    virtual int  FUNCTION_CALL_MODE WasNull()=0;

    ///取下一條記錄
    virtual void  FUNCTION_CALL_MODE Next()=0;

    ///判斷是否為結尾
    /**@return 1 是，0 不是;
      */
    virtual int  FUNCTION_CALL_MODE IsEOF()=0;
   
	///判斷是否為空
    /**@return 1 是，0 不是;
      */
    virtual int  FUNCTION_CALL_MODE IsEmpty()=0;
    
    virtual void * FUNCTION_CALL_MODE Destroy() = 0;
};

struct IF2UnPacker;

///多結果集打包接口(一個包可有多個異構結果集)
/**執行序列:
 *
 * 0、準備(可選, 若不進行該操作, 則由IF2Packer內部申請內存緩衝)： SetBuffer(),如果打包緩存區由調用者提供,則必須在BeginPack()之前準備;
 *
 * 1、開始:  BeginPack(),打包器復位;
 *
 * 2、第一個結果集打包：
 *
 *(a)添加字段名列表域：AddField()
 *
 *(b)按照結果集二維表順序，逐字段，逐條記錄按字段類型添加內容：AddStr() {AddInt(), AddDouble(), AddRaw()}
 *
 * 3、設置結果集的返回碼(可選，若不設置, 則返回碼為0) SetReturnCode()
 *
 * 4、打下一個結果集(可選) NewDataSet()，此處同時設置了該結果集的返回碼；
 * 
 * 5、參考第2步實現下一個結果集打包(可選)；
 *
 * 6、結束：EndPack(),重複調用會導致加入空結果集;
 *
 * 7、取打包結果(緩存區，緩存區大小，數據長度)
 *    打包結果也可以直接解包UnPack()返回解包接口
 *
 *使用注意事項:IF2Packer所使用的內存緩存區，由調用者負責回收；
 *             結果集附帶的返回碼，只有在包格式版本0x21及以上時有效；
 */
struct IF2Packer : public IKnown
{
    ///打包器初始化(使用調用者的緩存區)
	/** 第一次使用打包器時，可先使用本方法設置好緩衝區(數據長度被置為iDataLen)
	 *@param  char * pBuf  緩衝區地址
 	 *@param  int iBufSize  緩衝區空間
 	 *@param  int iDataLen  已有數據長度，新增數據加在已有數據之後（只對V1.0格式的包有效） 	 
 	 */
	virtual void FUNCTION_CALL_MODE SetBuffer(void * pBuf,int iBufSize,int iDataLen=0 )=0;

	///復位，重新開始打另一個包(字段數與記錄數置為0行0例)
	/**
	 * 功能：開始打包，把包長度清零(重複使用已有的緩存區空間)
	 *@return 無
	 */
	virtual void FUNCTION_CALL_MODE BeginPack(void)=0;

	///開始打一個結果集
	/**在打單結果集的包時，可以不調用本方法,均取默認值
	 *@param const char *szDatasetName 0x20版打包需要指明結果集名字
	 *@param int iReturnCode           0x20版打包需要為每個結果集指明返回值
	 */
	virtual int FUNCTION_CALL_MODE NewDataset(const char *szDatasetName, int iReturnCode = 0)=0;

	/**
	 * 功能：向包添加字段
	 *
	 *有執行次序要求:在 NewDataset()或Reset(),SetBuffer()之後,逐個字段按順序添加;
	 *
	 *@param szFieldName：字段名
	 *@param cFieldType ：字段類型:I整數，F浮點數，C字符，S字符串，R任意二進制數據
	 *@param iFieldWidth ：字段寬度（所占最大字節數）
	 *@param iFieldScale ：字段精度,即cFieldType='F'時的小數位數(缺省為4位小數)
	 *@return 負數表示失敗，否則為目前包的長度
	 */
	virtual int FUNCTION_CALL_MODE AddField(const char *szFieldName,char cFieldType ='S',int iFieldWidth=255,int iFieldScale=4)=0;

	/**
	 * 功能：向包添加字符串數據
     * 有執行次序要求:必須在所有字段增加完之後,逐個字段按順序添加;
	 *@param       szValue：字符串數據
	 *@return 負數表示失敗，否則為目前包的長度
	 */
	virtual int FUNCTION_CALL_MODE AddStr(const char *szValue)=0;

	/**
     * 功能：向包添加整數數據
 	 *@param       iValue：整數數據
	 *@return 負數表示失敗，否則為目前包的長度
	 */
	virtual int FUNCTION_CALL_MODE AddInt(int iValue)=0;
	
    /**
	 * 功能：向包添加浮點數據
	 *@param       fValue：浮點數據
	 *@return 負數表示失敗，否則為目前包的長度
	 */
	virtual int FUNCTION_CALL_MODE AddDouble(double fValue)=0;

	/**
	 * 功能：向包添加一個字符
	 *@param		 cValue：字符
	 *@return 負數表示失敗，否則為目前包的長度
	 */
	virtual int FUNCTION_CALL_MODE AddChar(char cValue)=0;

	/**
	 * 功能：向包添加一個大對象
	 *@param	void * lpBuff 數據區
	 *@param	int iLen  數據長度	 
	 *@return 負數表示失敗，否則為目前包的長度
	 */
	virtual int FUNCTION_CALL_MODE AddRaw(void * lpBuff,int iLen)=0;

    ///結束打包
	virtual void FUNCTION_CALL_MODE EndPack()=0;
 
	/**
     * 功能：取打包結果指針
	 *@return 打包結果指針
     */
	virtual void * FUNCTION_CALL_MODE GetPackBuf(void) = 0;
	
	/**
     * 功能：取打包結果長度
     *@return 打包結果長度
	 */
	virtual int FUNCTION_CALL_MODE GetPackLen(void) = 0;
	
	/**
	 * 功能：取打包結果緩衝區大小
     *@return 打包結果緩衝區大小
	 */
	virtual int FUNCTION_CALL_MODE GetPackBufSize(void) = 0;
	
	/**
	 * 功能：取打包格式版本
     *@return 版本
	 */
	virtual int FUNCTION_CALL_MODE GetVersion(void) = 0;
	
	///設置結果集的返回碼(0x20版以上要求)，錯誤結果集需要設置
	/**返回碼取缺省值0，則不設置，如果設置，則必須在EndPack()之前調用
     *@return 版本
	 */
	virtual void FUNCTION_CALL_MODE SetReturnCode(unsigned long dwRetCode) = 0;

	/**直接返回當前打包結果的解包接口,必須在EndPack()之後才能調用,在打包器釋放時相應的解包器實例也釋放
     *@return 解包器接口，此解包接口不能調用 destroy()來釋放
	 */
	virtual IF2UnPacker * FUNCTION_CALL_MODE UnPack(void) = 0;

	/**
	 *
	 */
	virtual void FUNCTION_CALL_MODE FreeMem(void* lpBuf) = 0;

	/**
	 *
	 */
	virtual void FUNCTION_CALL_MODE ClearValue() = 0;
	
	
	//20110302 xuxp 增加一個接口函數，用來傳遞第一個結果集的名字
	///復位，重新開始打另一個包(字段數與記錄數置為0行0例)
	/**
	 * 功能：開始打包，把包長度清零(重複使用已有的緩存區空間)
	 *@return 無
	 */
	virtual void FUNCTION_CALL_MODE BeginPackEx(char* szName = NULL) = 0;

	//20110324 dongpf 增加一個接口函數，用來復位當前結果集
	///復位當前結果集(字段數與記錄數置為0行0例)，不影響其他結果集
	/**
	 * 功能：復位當前結果集
	 *@return 無
	 */
	virtual void FUNCTION_CALL_MODE ClearDataSet() = 0;
};

///解包器接口
struct IF2UnPacker : public IF2ResultSet
{
	/**取打包格式版本
     *@return 版本
	 */
	virtual int FUNCTION_CALL_MODE GetVersion(void) = 0;

	/**取解包數據長度
     *@return 							0 表示成功， 其它為失敗
	 */
	virtual int FUNCTION_CALL_MODE Open(void * lpBuffer,unsigned int iLen) = 0;

    ///取結果集個數(0x20以上版本支持)
    virtual int FUNCTION_CALL_MODE GetDatasetCount()=0;

    ///設置當前結果集(0x20以上版本支持)
    /**
	 *@param  int nIndex				結果集編號
	 *@return int						非0 表示成功，否則為失敗
	 */
    virtual int FUNCTION_CALL_MODE SetCurrentDatasetByIndex(int nIndex)=0;

    ///設置當前結果集 (0x20以上版本支持)
    /**
	 *@param  const char *szDatasetName	結果集名稱
	 *@return int						非0 表示成功，否則為失敗
	 */
    virtual int FUNCTION_CALL_MODE SetCurrentDataset(const char *szDatasetName)=0;

	/** 取解包數據區指針
	 *@return 數據區指針
     */
	virtual void * FUNCTION_CALL_MODE GetPackBuf(void) = 0;

	/** 取解包數據長度
     *@return 解包數據長度
	 */
	virtual unsigned int FUNCTION_CALL_MODE GetPackLen(void) = 0;

	/**取解包數據記錄條數,20051207以後版本支持
     *@return 記錄條數
	 */
	virtual unsigned int FUNCTION_CALL_MODE GetRowCount(void) = 0;
	
	///結果集行記錄游標接口：取結果集的首條記錄
    virtual void FUNCTION_CALL_MODE First() = 0;

    ///結果集行記錄游標接口：取結果集的最後一條記錄
    virtual void FUNCTION_CALL_MODE Last() = 0;

    ///結果集行記錄游標接口：取結果集的第n條記錄，取值範圍[1, GetRowCount()]
    virtual void FUNCTION_CALL_MODE Go(int nRow) = 0;
	
	///獲取當前結果集名字的接口,沒有名字返回""
	virtual const char* FUNCTION_CALL_MODE GetDatasetName() = 0;
	
	virtual int FUNCTION_CALL_MODE OpenAndCopy(void * lpBuffer,unsigned int iLen) = 0;
	
	//20140623 majc 增加根據名字獲取字段類型，字段精度，字段最大長度
	//取字段數據類型
    /** @param columnName:字段名
      * @return:見上面常量定義;列名不存在 默認返回'S'
      */
    virtual char FUNCTION_CALL_MODE GetColTypeByName(const char * columnName)=0;


   ///取數字型字段小數位數
   /** @param columnName:字段名
     * @return int 返回字段數據小數位數 列名不存在 返回0 
     */
   virtual int FUNCTION_CALL_MODE GetColScaleByName(const char * columnName)=0;
  
   //取字段允許存放數據的最大寬度.
   /** @param columnName:字段名
     * @return int 返回字段寬度 列名不存在 返回1
     */
   virtual int FUNCTION_CALL_MODE GetColWidthByName(const char * columnName) = 0;
};

///連接對象 CConnectionInterface 的參數配置對象CConfigInterface
/**
* 包括從文件加載、保存到文件，讀寫操作
*/
class CConfigInterface: public IKnown
{
public:
	/**
    * 從文件加載
    * @param szFileName 文件名，格式類似ini，具體參考開發包示例
    * @return 返回0表示成功，否則失敗
    */
    virtual int FUNCTION_CALL_MODE Load(const char *szFileName) = 0;

    /**
    * 保存到文件
    * @param szFileName 文件名
    * @return 返回0表示成功，否則失敗
    */
    virtual int FUNCTION_CALL_MODE Save(const char *szFileName) = 0;

    /**
    * 取字符串值
    * @param szSection 節名
    * @param szEntry   變量名
    * @param szDefault 缺省值
    * @return 字符串值，沒有找到時返回szDefault
    */
    virtual const char * FUNCTION_CALL_MODE GetString(const char *szSection, const char *szEntry, const char *szDefault) = 0;

    /**
    * 取整數值
    * @param szSection 節名
    * @param szEntry   變量名
    * @param iDefault  缺省值
    * @return 整數值，沒有找到時返回iDefault
    */
    virtual int FUNCTION_CALL_MODE GetInt(const char *szSection, const char *szEntry, int iDefault) = 0;

    /**
    * 設置字符串值
    * @param szSection 節名
    * @param szEntry   變量名
    * @param szValue   值
    * @return 0表示成功，否則失敗
    */
    virtual int FUNCTION_CALL_MODE SetString(const char *szSection, const char *szEntry, const char *szValue) = 0;

    /**
    * 設置整數值
    * @param szSection 節名
    * @param szEntry   變量名
    * @param iValue    值
    * @return 0表示成功，否則失敗
    */
    virtual int FUNCTION_CALL_MODE SetInt(const char *szSection, const char *szEntry, int iValue) = 0;
};


typedef struct tagBizRouteInfo
{
	char ospfName[ID_STR_LEN+1];//路由目標節點中間件名字
	char nbrName[ID_STR_LEN+1];//中間件節點的鄰居名字
	char svrName[SVRINSTANCE_NAME_LENGTH+1];//中間件的進程名字
	char pluginID[PLUGIN_NAME_LENGTH+1];//中間件插件名
	int connectID;//連接號
	int memberNO;//成員編號
	
	tagBizRouteInfo()
	{
		memset(this,0,sizeof(tagBizRouteInfo));
	}
}BIZROUTE_INFO;


struct IBizMessage : public IKnown
{
	//設置功能號
	virtual void FUNCTION_CALL_MODE SetFunction(const int nFUnctionNo) = 0;
	//獲取功能號
	virtual int FUNCTION_CALL_MODE GetFunction() = 0;

	//設置包類型
	virtual void FUNCTION_CALL_MODE SetPacketType(const int nPacketType) = 0;
	//獲取包類型
	virtual int FUNCTION_CALL_MODE GetPacketType() = 0;

	//設置營業部號
	virtual void FUNCTION_CALL_MODE SetBranchNo(const int nBranchNo) = 0;
	//獲取營業部號
	virtual int FUNCTION_CALL_MODE GetBranchNo() = 0;

	//設置系統號
	virtual void FUNCTION_CALL_MODE SetSystemNo(const int nSystemNo) = 0;
	//獲取系統號
	virtual int FUNCTION_CALL_MODE GetSystemNo() = 0;

	//設置子系統號
	virtual void FUNCTION_CALL_MODE SetSubSystemNo(const int nSubSystemNo) = 0;
	//獲取子系統號
	virtual int FUNCTION_CALL_MODE GetSubSystemNo() = 0;

	//設置發送者編號
	virtual void FUNCTION_CALL_MODE SetSenderId(const int nSenderId) = 0;
	//獲取發送者編號
	virtual int FUNCTION_CALL_MODE GetSenderId() = 0;

	//設置包序號
	virtual void FUNCTION_CALL_MODE SetPacketId(const int nPacketId) = 0;
	//獲取包序號
	virtual int FUNCTION_CALL_MODE GetPacketId() = 0;

	//設置目的地路由
	virtual void FUNCTION_CALL_MODE SetTargetInfo(const BIZROUTE_INFO targetInfo) = 0;
	//獲取目的地路由
	virtual void FUNCTION_CALL_MODE GetTargetInfo(BIZROUTE_INFO& targetInfo) = 0;
	
	//設置發送者路由
	virtual void FUNCTION_CALL_MODE SetSendInfo(const BIZROUTE_INFO sendInfo) = 0;
	//獲取發送者路由
	virtual void FUNCTION_CALL_MODE GetSendInfo(BIZROUTE_INFO& sendInfo) = 0;

	//設置錯誤號
	virtual void FUNCTION_CALL_MODE SetErrorNo(const int nErrorNo) = 0;
	//獲取錯誤號
	virtual int FUNCTION_CALL_MODE GetErrorNo() = 0;
	
	//設置錯誤信息
	virtual void FUNCTION_CALL_MODE SetErrorInfo(const char* strErrorInfo) = 0;
	//獲取錯誤信息
	virtual const char* FUNCTION_CALL_MODE GetErrorInfo() = 0;
	
	//設置返回碼
	virtual void FUNCTION_CALL_MODE SetReturnCode(const int nReturnCode) = 0;
	//獲取返回碼
	virtual int FUNCTION_CALL_MODE GetReturnCode() = 0;

	//設置業務內容
	virtual void FUNCTION_CALL_MODE SetContent(void* lpContent,int iLen) = 0;
	//獲取業務內容
	virtual const void* FUNCTION_CALL_MODE GetContent(int& iLen) = 0;

	//以下接口用於消息中心1.0的訂閱
	//設置訂閱類型
	virtual void FUNCTION_CALL_MODE SetIssueType(const int nIssueType) = 0;
	//獲取訂閱類型
	virtual int FUNCTION_CALL_MODE GetIssueType() = 0;

	//設置序號
	virtual void FUNCTION_CALL_MODE SetSequeceNo(const int nSequeceNo) = 0;
	//獲取序號
	virtual int FUNCTION_CALL_MODE GetSequeceNo() = 0;

	//設置關鍵字段信息
	virtual void FUNCTION_CALL_MODE SetKeyInfo(void* lpKeyData,int iLen) = 0;
	//獲取關鍵字段信息
	virtual const void* FUNCTION_CALL_MODE GetKeyInfo(int& iLen) = 0;

	//設置附加數據，訂閱推送時原樣返回
	virtual void FUNCTION_CALL_MODE SetAppData(const void* lpAppdata,int nAppLen) = 0;
	//獲取附加數據，訂閱推送時原樣返回
	virtual const void* FUNCTION_CALL_MODE GetAppData(int& nAppLen) = 0;

	//請求轉應答
	virtual int	FUNCTION_CALL_MODE ChangeReq2AnsMessage() = 0;

	//獲取二進制
	virtual void* FUNCTION_CALL_MODE GetBuff(int& nBuffLen) = 0;
	//解析二進制
	virtual int	FUNCTION_CALL_MODE SetBuff(const void* lpBuff,int nBuffLen) = 0;

	//清除消息內的字段，可以下次復用。
	virtual void FUNCTION_CALL_MODE ReSet() = 0;
};

#define IDENTITY_NAME_LENGTH    32  /**< 客戶端名字長度 */
#define MAX_MACADDRESS_LEN	    18  /**< MAC 地址長度 */
#define MAX_RAND_LEN	        4   /**< 隨機數長度 */

/** 客戶標識長度 */
#define MAX_BIZNAME_LEN \
	IDENTITY_NAME_LENGTH+1+MAX_MACADDRESS_LEN+1+MAX_RAND_LEN+2

#define INIT_RECVQ_LEN 256          /**< 接收隊列初始長度 */
#define STEP_RECVQ_LEN 512          /**< 接收隊列擴展步長 */
#define SIMPLIFIED_CHINESE      0   /**< 錯誤信息語言:簡體中文 */
#define ENGLISH                 1   /**< 錯誤信息語言:英文 */
#define MAX_FILTERFIELD_LEN 63      /**< 過濾字段長度 */

/** 主題可靠等級 */
enum ReliableLevel
{
    LEVEL_DOBEST            = 0,    /**< 盡力而為 */
    LEVEL_DOBEST_BYSEQ      = 1,    /**< 盡力有序 */
    LEVEL_MEM               = 2,    /**< 內存 */
    LEVEL_FILE              = 3,    /**< 文件 */
    LEVEL_SYSTEM            = 4     /**< 系統 */
};

/** 
 * 過濾器接口
 */
class CFilterInterface:public IKnown
{
public:
   /**
    * 根據下標獲取過濾條件的名字
    * @param index 對應的過濾條件下標
    * @return 返回對應的下標過濾條件的名字，否則返回NULL.
    */
    virtual char* FUNCTION_CALL_MODE GetFilterNameByIndex(int index) = 0;

   /**
    * 根據下標獲取過濾條件的值
    * @param index 對應的過濾條件下標
    * @return 返回對應的下標過濾條件的值，否則返回NULL.
    */
    virtual char* FUNCTION_CALL_MODE GetFilterValueByIndex(int index)= 0;

   /**
    * 根據過濾條件的名字獲取過濾條件的值
    * @param fileName 對應的過濾條件名字
    * @return 返回對應的過濾條件名字的條件值，否則返回NULL.
    */
    virtual char* FUNCTION_CALL_MODE GetFilterValue(char*  fileName)= 0;

   /**
    * 獲取過濾條件的個數
    * @return 返回對應過濾條件的個數，沒有返回0
    */
    virtual int   FUNCTION_CALL_MODE GetCount() = 0;

   /**
    * 設置過濾條件，根據過濾條件名字和值
    * @param filterName 對應的過濾條件名字
    * @param filterValue 對應的過濾條件名字的值
    */
    virtual void FUNCTION_CALL_MODE  SetFilter(char* filterName,char* filterValue) =0;
};

/**
 * 訂閱參數類接口
 */
class CSubscribeParamInterface:public IKnown
{
public:

   /**
    * 設置主題名字
    * @param szName 對應的主題名字
    */
    virtual void FUNCTION_CALL_MODE  SetTopicName(char* szName) =0;

   /**
    * 設置附加數據
    * @param lpData 附加數據的首地址
    * @param iLen 附加數據的長度
    */
    virtual void FUNCTION_CALL_MODE  SetAppData(void* lpData,int iLen)=0;
    
   /**
    * 添加過濾條件
    * @param filterName 過濾條件的名字
    * @param filterValue 過濾條件的值
    */
    virtual void FUNCTION_CALL_MODE  SetFilter(char* filterName,char* filterValue)=0;

   /**
    * 添加返回字段
    * @param filedName 需要添加的返回字段
    */
    virtual void FUNCTION_CALL_MODE  SetReturnFiled(char* filedName)=0;

   /**
    * 設置是否補缺標誌
    * @param bFromNow true表示需要之前的數據，也就是補缺，false表示不需要補缺
    */
    virtual void FUNCTION_CALL_MODE  SetFromNow(bool bFromNow)=0;

   /**
    * 設置覆蓋訂閱標誌
    * @param bReplace true表示覆蓋訂閱，取消之前的所有訂閱，只保留當前的訂閱，false表示追加訂閱
    */
    virtual void FUNCTION_CALL_MODE  SetReplace(bool bReplace)=0;

   /**
    * 設置發送間隔
    * @param nSendInterval 單位是秒
    */
    virtual void FUNCTION_CALL_MODE  SetSendInterval(int nSendInterval)=0;

   /**
    * 獲取主題名字
    * @return 返回主題名字信息
    */
    virtual char* FUNCTION_CALL_MODE  GetTopicName() =0;

   /**
    * 獲取附加數據
    * @param iLen 出參，表示附加數據的長度
    * @return 返回附加數據首地址，沒有返回NULL
    */
    virtual void* FUNCTION_CALL_MODE  GetAppData(int *iLen) =0;

   /**
    * 獲取對應的過濾字段的名字
    * @param index 對應的過濾條件下標
    * @return 返回對應的下標過濾條件的名字，否則返回NULL.
    */
    virtual char* FUNCTION_CALL_MODE  GetFilterNameByIndex(int index) = 0;

   /**
    * 根據下標獲取過濾條件的值
    * @param index 對應的過濾條件下標
    * @return 返回對應的下標過濾條件的值，否則返回NULL.
    */
    virtual char* FUNCTION_CALL_MODE  GetFilterValueByIndex(int index)= 0;

   /**
    * 根據過濾條件的名字獲取過濾條件的值
    * @param fileName 對應的過濾條件名字
    * @return 返回對應的過濾條件名字的條件值，否則返回NULL.
    */
    virtual char* FUNCTION_CALL_MODE  GetFilterValue(char*  fileName)= 0;

   /**
    * 獲取過濾條件的個數
    * @return 返回對應過濾條件的個數，沒有返回0
    */
    virtual int   FUNCTION_CALL_MODE  GetFilterCount() = 0;

   /**
    * 獲取返回字段
    * @return 返回對應的返回字段信息
    */
    virtual char* FUNCTION_CALL_MODE  GetReturnFiled()=0;

   /**
    * 獲取是否補缺的標誌
    * @return 返回對應的補缺標誌
    */
    virtual bool  FUNCTION_CALL_MODE  GetFromNow()=0 ;

   /**
    * 獲取是否覆蓋訂閱的標誌
    * @return 返回對應的覆蓋訂閱標誌
    */
    virtual bool  FUNCTION_CALL_MODE  GetReplace() =0;

   /**
    * 獲取對應的發送頻率
    * @return 返回對應的發送間隔
    */
    virtual int   FUNCTION_CALL_MODE  GetSendInterval()=0;
};

class CSubscribeInterface;

/**
 * @brief 訂閱回調接口返回的數據定義，除了訂閱需要的業務體之外，還需要返回的數據
 */
typedef struct tagSubscribeRecvData
{
	char* lpFilterData;     /**< 過濾字段的數據頭指針，用解包器解包 */
	int iFilterDataLen;     /**< 過濾字段的數據長度 */
	char* lpAppData;        /**< 附加數據的數據頭指針 */
	int iAppDataLen;        /**< 附加數據的長度 */
	char szTopicName[260];  /**< 主題名字 */
	
   /**
    * tagSubscribeRecvData 構造函數
    */
	tagSubscribeRecvData()
	{
		memset(this,0,sizeof(tagSubscribeRecvData));
	}
}SUBSCRIBE_RECVDATA, *LPSUBSCRIBE_RECVDATA;

/**
 * 訂閱回調接口，上層應用通過這個接口，接收主推過來的消息
 */
class CSubCallbackInterface: public IKnown
{
public:

   /**
    * 收到發布消息的回調
    * @param lpSub 回調的訂閱指針
    * @param subscribeIndex 消息對應的訂閱標識，這個標識來自於SubscribeTopic函數的返回
    * @param lpData 返回消息的二進制指針，一般是消息的業務體打包內容
    * @param nLength 二進制數據的長度
	* @param lpRecvData 主推消息的其他字段返回，主要包含了附加數據，過濾信息，主題名字，詳細參看前面結構體定義
    * @return 無
    */
    virtual void FUNCTION_CALL_MODE OnReceived(CSubscribeInterface *lpSub,int subscribeIndex, const void *lpData, int nLength,LPSUBSCRIBE_RECVDATA lpRecvData) = 0;

   /**
    * 收到剔除訂閱項的消息回調，一般在擁有踢人策略的主題下會回調這個接口,這個回調裡面不需要取消訂閱，底層已經取消這個訂閱，只是一個通知接口
    * @param lpSub 回調的訂閱指針
    * @param subscribeIndex 消息對應的訂閱標識，這個標識來自於SubscribeTopic函數的返回
    * @param TickMsgInfo 踢人的錯誤信息，主要是包含具體重複的訂閱項位置信息
    * @return 無
    */
    virtual void FUNCTION_CALL_MODE OnRecvTickMsg(CSubscribeInterface *lpSub,int subscribeIndex,const char* TickMsgInfo) = 0;
};


/**
 * 訂閱接口的定義
 */
class CSubscribeInterface: public IKnown
{
public:

   /**
    * 訂閱主題
    * @param lpSubscribeParamInter 上面定義的訂閱參數結構
    * @param uiTimeout 超時時間
    * @param lppBizUnPack 業務校驗時，失敗返回的業務錯誤信息，如果訂閱成功沒有返回，輸出參數，需要外面調用Release釋放
                          如果接受業務校驗的錯誤信息，寫法如下：
                          IF2UnPacker* lpBizUnPack =NULL;
                          SubscribeTopic(...,&lpBizUnPack);
                          最後根據返回值，如果是失敗的就判斷 lpBizUnPack 是不是NULL.
                          最後錯誤信息獲取完之後,釋放
                          lpBizUnPack->Release();
    * @param lpBizPack 業務校驗需要增加的業務字段以及值，沒有就根據過濾屬性作為業務校驗字段
    * @return 返回值大於0，表示當前訂閱成功的訂閱標識，外面要記住這個標識和訂閱項之間的映射關係，這個標識需要用於取消訂閱和接收消息的回調裡面.
	*		  返回其他值，根據錯誤號獲取錯誤信息.
    */
    virtual int FUNCTION_CALL_MODE SubscribeTopic(CSubscribeParamInterface* lpSubscribeParamInter ,unsigned int uiTimeout,IF2UnPacker** lppBizUnPack=NULL,IF2Packer* lpBizPack=NULL) = 0;

   /**
    * 取消訂閱主題
    * @param subscribeIndex 消息對應的訂閱標識，這個標識來自於SubscribeTopic函數的返回
    * @return 返回0表示取消訂閱成功，返回其他值，根據錯誤號獲取錯誤信息.
    */
    virtual int FUNCTION_CALL_MODE CancelSubscribeTopic(int subscribeIndex) = 0;


	/**
    * 取消訂閱主題
    * @param topicName 主題名字
    * @param lpFilterInterface 對應的過濾條件
    * @return 返回0表示取消訂閱成功，返回其他值，根據錯誤號獲取錯誤信息.
    */
    virtual int FUNCTION_CALL_MODE CancelSubscribeTopicEx(char* topicName,CFilterInterface* lpFilterInterface) = 0;



   /**
    * 獲取當前訂閱接口已經訂閱的所有主題以及過濾條件信息
    * @param lpPack 外面傳入的打包器
    * @note packer報文字段
        - SubcribeIndex
        - IsBornTopic
        - TopicName
        - TopicNo
        - FilterRaw
        - Appdata
        - SendInterval
        - ReturnFileds
        - isReplace
        - isFromNow
    */
    virtual void FUNCTION_CALL_MODE GetSubcribeTopic(IF2Packer* lpPack)=0;
};

/**
 * 發布接口
 */
class CPublishInterface: public IKnown
{
public:

   /**
    * 業務打包格式的內容發布接口
    * @param topicName 主題名字，不知道名字就傳NULL
    * @param lpUnPacker 具體的內容
    * @param iTimeOut 超時時間
    * @param lppBizUnPack 業務校驗時，失敗返回的業務錯誤信息，如果發布成功沒有返回，輸出參數，需要外面調用Release釋放
                            如果接受業務校驗的錯誤信息，寫法如下：
                            IF2UnPacker* lpBizUnPack =NULL;
                            PubMsgByPacker(...,&lpBizUnPack);
                            最後根據返回值，如果是失敗的就判斷 lpBizUnPack 是不是NULL.
                            最後錯誤信息獲取完之後,釋放
                            lpBizUnPack->Release();
    * @param bAddTimeStamp 是否添加時間戳，配合單筆性能查找
    * @return 返回0表示成功，返回其他值，根據錯誤號獲取錯誤信息.
    */
    virtual int FUNCTION_CALL_MODE PubMsgByPacker(char* topicName ,IF2UnPacker* lpUnPacker,int iTimeOut=-1,
        IF2UnPacker** lppBizUnPack=NULL,bool bAddTimeStamp=false) = 0;
    
   /**
    * 非業務打包格式的內容發布接口，一般二進制格式報文發布
    * @param topicName 主題名字，不知道名字就傳NULL
    * @param lpFilterInterface 過濾條件，需要上層自己指定，否則默認沒有過濾條件
    * @param lpData 具體的內容
    * @param nLength 內容長度
    * @param iTimeOut 超時時間
    * @param lppBizUnPack 業務校驗時，失敗返回的業務錯誤信息，如果發布成功沒有返回，輸出參數，需要外面調用Release釋放
                            如果接受業務校驗的錯誤信息，寫法如下：
                            IF2UnPacker* lpBizUnPack =NULL;
                            PubMsgByPacker(...,&lpBizUnPack);
                            最後根據返回值，如果是失敗的就判斷 lpBizUnPack 是不是NULL.
                            最後錯誤信息獲取完之後,釋放
                            lpBizUnPack->Release();
    * @param bAddTimeStamp 是否添加時間戳，配合單筆性能查找
    * @return 返回0表示成功，返回其他值，根據錯誤號獲取錯誤信息.
    */
    virtual int FUNCTION_CALL_MODE PubMsg(char* topicName, CFilterInterface* lpFilterInterface, const void *lpData, int nLength,int iTimeOut=-1,
        IF2UnPacker** lppBizUnPack=NULL,bool bAddTimeStamp=false) = 0;

   /**
    * 返回當前主題的發布序號
    * @param topicName 主題名字
    * @return 返回0表示沒有對應的主題，返回其他值表示成功
    */
    virtual uint32 FUNCTION_CALL_MODE GetMsgNoByTopicName(char* topicName)=0;

   /**
    * 取服務器地址
    * @param lpPort 輸出的服務器端口，可以為NULL
    * @return 返回服務器地址
    */
    virtual const char * FUNCTION_CALL_MODE GetServerAddress(int *lpPort) = 0;
};

class CConnectionInterface;

///連接對象 CConnectionInterface 需要的回調對象接口定義
/**
* 包括連接成功、連接斷開、發送完成、收到數據等事件    
*/
class CCallbackInterface: public IKnown
{
public:
    /**
    * 套接字連接成功
    * @param lpConnection 發生該事件的連接對象
    */
    virtual void FUNCTION_CALL_MODE OnConnect(CConnectionInterface *lpConnection) = 0;

    /**
    * 完成安全連接
    * @param lpConnection 發生該事件的連接對象
    */
    virtual void FUNCTION_CALL_MODE OnSafeConnect(CConnectionInterface *lpConnection) = 0;

    /**
    * 完成註冊
    * @param lpConnection 發生該事件的連接對象
    */
    virtual void FUNCTION_CALL_MODE OnRegister(CConnectionInterface *lpConnection) = 0;

    /**
    * 連接被斷開
    * @param lpConnection 發生該事件的連接對象
    */
    virtual void FUNCTION_CALL_MODE OnClose(CConnectionInterface *lpConnection) = 0;

    /**
    * 發送完成
    * @param lpConnection 發生該事件的連接對象
    * @param hSend        發送句柄
    * @param reserved1    保留字段
    * @param reserved2    保留字段
    * @param nQueuingData 發送隊列中剩餘個數，使用者可以用此數值控制發送的速度，即小於某值時進行發送
    * @see Send()
    */
    virtual void FUNCTION_CALL_MODE OnSent(CConnectionInterface *lpConnection, int hSend, void *reserved1, void *reserved2, int nQueuingData) = 0;

    virtual void FUNCTION_CALL_MODE Reserved1(void *a, void *b, void *c, void *d) = 0;

    virtual void FUNCTION_CALL_MODE Reserved2(void *a, void *b, void *c, void *d) = 0;

    virtual int  FUNCTION_CALL_MODE Reserved3() = 0;

    virtual void FUNCTION_CALL_MODE Reserved4() = 0;

    virtual void FUNCTION_CALL_MODE Reserved5() = 0;

    virtual void FUNCTION_CALL_MODE Reserved6() = 0;

    virtual void FUNCTION_CALL_MODE Reserved7() = 0;

    /**
    * 收到SendBiz異步發送的請求的應答
    * @param lpConnection    發生該事件的連接對象
    * @param hSend           發送句柄
    * @param lpUnPackerOrStr 指向解包器指針或者錯誤信息
    * @param nResult         收包結果
    * 如果nResult等於0，表示業務數據接收成功，並且業務操作成功，lpUnPackerOrStr指向一個解包器，此時應首先將該指針轉換為IF2UnPacker *。
    * 如果nResult等於1，表示業務數據接收成功，但業務操作失敗了，lpUnPackerOrStr指向一個解包器，此時應首先將該指針轉換為IF2UnPacker *。
    * 如果nResult等於2，表示收到非業務錯誤信息，lpUnPackerOrStr指向一個可讀的字符串錯誤信息。
    * 如果nResult等於3，表示業務包解包失敗。lpUnPackerOrStr指向NULL。
    */
    virtual void FUNCTION_CALL_MODE OnReceivedBiz(CConnectionInterface *lpConnection, int hSend, const void *lpUnPackerOrStr, int nResult) = 0;

		/**
    * 收到SendBiz異步發送的請求的應答
    * @param lpConnection    發生該事件的連接對象
    * @param hSend           發送句柄
    * @param lpRetData 其他需要返回的應答內容，根據需要獲取
    * @param lpUnPackerOrStr 指向解包器指針或者錯誤信息
    * @param nResult         收包結果
    * 如果nResult等於0，表示業務數據接收成功，並且業務操作成功，lpUnpackerOrStr指向一個解包器，此時應首先將該指針轉換為IF2UnPacker *。
    * 如果nResult等於1，表示業務數據接收成功，但業務操作失敗了，lpUnpackerOrStr指向一個解包器，此時應首先將該指針轉換為IF2UnPacker *。
    * 如果nResult等於2，表示收到非業務錯誤信息，lpUnpackerOrStr指向一個可讀的字符串錯誤信息。
    * 如果nResult等於3，表示業務包解包失敗。lpUnpackerOrStr指向NULL。
    */
    virtual void FUNCTION_CALL_MODE OnReceivedBizEx(CConnectionInterface *lpConnection, int hSend, LPRET_DATA lpRetData, const void *lpUnpackerOrStr, int nResult) = 0;
	//20130624 xuxp 回調增加BizMessage接口
		/**
		* 收到發送時指定了ReplyCallback選項的請求的應答或者是沒有對應請求的數據
		* @param lpConnection 發生該事件的連接對象
		* @param hSend        發送句柄
		* @param lpMsg        業務消息指針
		*/
	virtual void FUNCTION_CALL_MODE OnReceivedBizMsg(CConnectionInterface *lpConnection, int hSend, IBizMessage* lpMsg) = 0;



};

///T2_SDK連接對象接口
/**
* 連接的對象是線程不安全，同時一個連接最好是歸於一個線程所用，不要多線程使用！！！
* 包括連接、斷開、發送、接收等
*/
class CConnectionInterface: public IKnown
{
public:
    ///連接狀態，可組合
    enum ConnectionStatus
    {
        Disconnected	= 0x0000, /**< 未連接 */
        Connecting		= 0x0001, /**< socket正在連接 */
        Connected		= 0x0002, /**< socket已連接 */
        SafeConnecting	= 0x0004, /**< 正在建立安全連接 */
        SafeConnected	= 0x0008, /**< 已建立安全連接 */
        Registering		= 0x0010, /**< 正註冊 */
        Registered		= 0x0020, /**< 已註冊 */
        Rejected		= 0x0040  /**< 被拒絕，將被關閉 */
    };

    ///接收選項（可組合，0表示接收超時時，不刪除包ID，仍可再次調用RecvBiz方法來嘗試接收）
    enum RecvFlags
    {
        JustRemoveHandle = 0x0001   /**< 當接收超時時，把packet_id刪除 */
    };

    /**
    * 初始化連接對象
    * @param lpCallback 回調對象
    * @return 返回0表示成功，否則表示失敗，通過調用GetErrorMsg可以獲取詳細錯誤信息
    * 如果應用不需要任何回調方法，則可向該方法傳遞NULL，而不必自定義回調類和對象
    */
    virtual int FUNCTION_CALL_MODE Create(CCallbackInterface *lpCallback) = 0;

    /**
    * 開始連接/註冊
    * @param uiTimeout 超時時間，單位毫秒，0表示不等待（使用代理連接服務器時，該參數不起作用）
    * @return 返回0表示成功，否則表示失敗，通過調用GetErrorMsg可以獲取詳細錯誤信息
    */
    virtual int FUNCTION_CALL_MODE Connect(unsigned int uiTimeout) = 0;

    /**
    * 斷開連接
    * @return 返回0表示成功，否則表示失敗，通過調用GetErrorMsg可以獲取詳細錯誤信息
    */
    virtual int FUNCTION_CALL_MODE Close() = 0;

    virtual int FUNCTION_CALL_MODE Reserved1() = 0;

    virtual int FUNCTION_CALL_MODE Reserved2() = 0;

    virtual int FUNCTION_CALL_MODE Reserved3() = 0;

    virtual int FUNCTION_CALL_MODE Reserved4() = 0;

    virtual int FUNCTION_CALL_MODE Reserved5() = 0;

    virtual int FUNCTION_CALL_MODE Reserved6() = 0;

    /**
    * 取服務器地址
    * @param lpPort 輸出的服務器端口，可以為NULL
    * @return 返回服務器地址
    */
    virtual const char * FUNCTION_CALL_MODE GetServerAddress(int *lpPort) = 0;

    /**
    * 取連接狀態
    * @return 返回連接狀態
    */
    virtual int FUNCTION_CALL_MODE GetStatus() = 0;

    /**
    * 取服務器負載，使用者可以同時創建多個連接實例同時連接不同的服務器，根據完成連接的時間以及負載決定使用哪個服務器
    * @return 返回服務器負載（非負數），越大表示越繁忙，否則表示失敗，通過調用GetErrorMsg可以獲取詳細錯誤信息
    */
    virtual int FUNCTION_CALL_MODE GetServerLoad() = 0;

    /**
    * 取錯誤碼對應的錯誤信息，目前支持簡體中文和英文，支持其他語言可以通過連接對象配置errormsg
    * @param nErrorCode 錯誤碼
    * @return 返回錯誤信息
    */
    virtual const char * FUNCTION_CALL_MODE GetErrorMsg(int nErrorCode) = 0;

    /**
    * 取連接錯誤號，當連接無法與服務端完成註冊時，既可通過Connect的返回值獲取錯誤號，
    * 也可通過調用本方法來獲取錯誤號，然後用該錯誤號調用GetErrorMsg可獲取可讀的錯誤信息
    * @return 返回連接錯誤號
    */
    virtual int FUNCTION_CALL_MODE GetConnectError() = 0;

    /**
    * 發送業務數據
    * @param iFunID      功能號
    * @param lpPacker    打包器指針
    * @param nAsy        0表示同步，否則表示異步
    * @param iSystemNo   如果iSystemNo > 0則設置系統號
    * @param nCompressID 對業務包體進行壓縮的壓縮算法ID，目前只支持ID = 1的壓縮算法。
    * ID = 0表示不壓縮。注意，壓縮只是向SDK提出建議，是否真正壓縮還取決於包的實際大小。
    * 同步發送的包，通過調用RecvBiz來接收，異步發送的包，當收到應答包後，自動觸發回調函數OnReceivedBiz。
    * @return 返回發送句柄（正數），否則表示失敗，通過調用GetErrorMsg可以獲取詳細錯誤信息
    */
    virtual int FUNCTION_CALL_MODE SendBiz(int iFunID, IF2Packer *lpPacker, int nAsy = 0, int iSystemNo = 0, int nCompressID = 1) = 0;

    /**
    * 接收業務數據
    * @param hSend            發送句柄（SendBiz的成功返回值）
    * @param lppUnPackerOrStr 如果返回值等於0，表示業務數據接收成功，並且業務操作成功，*lppUnPackerOrStr指向一個解包器，此時應首先將該指針轉換為IF2UnPacker *。
    *                         如果返回值等於1，表示業務數據接收成功，但業務操作失敗了，*lppUnPackerOrStr指向一個解包器，此時應首先將該指針轉換為IF2UnPacker *。
    *                         如果返回值等於2，表示收到非業務錯誤信息，*lppUnPackerOrStr指向一個可讀的字符串錯誤信息。
    *                         如果返回值等於3，表示業務包解包失敗。*lppUnPackerOrStr原先所指向的內容不會被改變。
    * @param uiTimeout        超時時間，單位毫秒。
    * @param uiFlag           接收選項，0表示接收超時後仍可繼續調用RecvBiz來接收，
    *                         JustRemoveHandle表示當接收超時後，把hSend相關數據刪除
    * @return 小於0表示RecvBiz操作本身失敗，通過調用GetErrorMsg可以獲取詳細錯誤信息
    * 注意！外部指針所指向的解包器的內存由SDK內部管理，外部切勿釋放！
	* 注意！lppUnPackerOrStr對應的解包器是臨時的，上層不可以緩存指針，再次調用這個連接的RecvBiz，指針指向的內容就會改變
	* 如果要拷貝，需要調用解包器的GetPackBuf方法，二進制拷貝出去，其他線程需要再解包
    */
    virtual int FUNCTION_CALL_MODE RecvBiz(int hSend, void **lppUnPackerOrStr, unsigned uiTimeout = 1000, unsigned uiFlag = 0) = 0;
    
   //20101228 xuxp 新增發送和接受的兩個接口函數，用於訂閱和推送
	/**
    * 發送業務數據
    * @param iFunID      功能號
    * @param lpPacker    打包器指針
	* @param svrName     指定中間件的節點
    * @param nAsy        0表示同步，否則表示異步。
    * @param iSystemNo   如果iSystemNo > 0則設置系統號
    * @param nCompressID 對業務包體進行壓縮的壓縮算法ID，目前只支持ID = 1的壓縮算法。
    * ID = 0表示不壓縮。注意，壓縮只是向SDK提出建議，是否真正壓縮還取決於包的實際大小。
    * 同步發送的包，通過調用RecvBizEx來接收，異步發送的包，當收到應答包後，自動觸發回調函數OnReceivedBizEx。
    * @param branchNo  營業部號。
    * @param lpRequest  請求裡面的其他內容，根據結構體定義賦值。
    * @return 返回發送句柄（正數），否則表示失敗，通過調用GetErrorMsg可以獲取詳細錯誤信息
    */
    virtual int FUNCTION_CALL_MODE SendBizEx(int iFunID, IF2Packer *lpPacker,char* svrName, int nAsy = 0, int iSystemNo = 0, int nCompressID = 1,int branchNo=0,LPREQ_DATA lpRequest=NULL) = 0;
    
    /**
    * 接收業務數據
    * @param hSend            發送句柄（SendBiz的成功返回值）
    * @param lppUnPackerOrStr 如果返回值等於0，表示業務數據接收成功，並且業務操作成功，*lppUnPackerOrStr指向一個解包器，此時應首先將該指針轉換為IF2UnPacker *。
    *                         如果返回值等於1，表示業務數據接收成功，但業務操作失敗了，*lppUnPackerOrStr指向一個解包器，此時應首先將該指針轉換為IF2UnPacker *。
    *                         如果返回值等於2，表示收到非業務錯誤信息，*lppUnPackerOrStr指向一個可讀的字符串錯誤信息。
    *                         如果返回值等於3，表示業務包解包失敗。*lppUnPackerOrStr原先所指向的內容不會被改變。
    * @param lpRetData 其他需要返回的應答內容，根據需要獲取
    * @param uiTimeout        超時時間，單位毫秒，0表示不等待。
    * @param uiFlag           接收選項，0表示接收超時後仍可繼續調用RecvBiz來接收，
    *                         JustRemoveHandle表示當接收超時後，把hSend相關數據刪除
    * @return 小於0表示RecvBizEx操作本身失敗，通過調用GetErrorMsg可以獲取詳細錯誤信息
    * 注意！外部指針所指向的解包器的內存由SDK內部管理，外部切勿釋放！
	* 注意！外部指針所指向的LPRET_DATA的內存由SDK內部管理，外部切勿釋放！
	* 注意！lppUnPackerOrStr對應的解包器和LPRET_DATA都是臨時的，上層不可以緩存指針，再次調用這個連接的RecvBizEx，這兩個指針指向的內容就會改變
	* 如果要拷貝，需要調用解包器的GetPackBuf方法，二進制拷貝出去，其他線程需要再解包；
	* LPRET_DATA拷貝，就需要做結構體複製
    */
    virtual int FUNCTION_CALL_MODE RecvBizEx(int hSend, void **lppUnpackerOrStr, LPRET_DATA* lpRetData, unsigned uiTimeout = 1000, unsigned uiFlag = 0) = 0;


	//20101228 xuxp 新增Create函數，來保證回調用新的OnReceivedBizEx的接口
	/**
	* 初始化連接對象
	* @param lpCallback 回調對象
	* @return 返回0表示成功，否則表示失敗，通過調用GetErrorMsg可以獲取詳細錯誤信息
    * 如果應用不需要任何回調方法，則可向該方法傳遞NULL，而不必自定義回調類和對象
	*/
	virtual int FUNCTION_CALL_MODE CreateEx(CCallbackInterface *lpCallback) = 0;
	
	
	//20120111 dongpf 新增GetRealAddress函數，來獲取服務端上自己的ip地址和端口
	/**
	* 獲取ip地址和端口
	* @return 獲取ip地址和端口，格式：ip地址+端口
	*/
	virtual const char* FUNCTION_CALL_MODE GetRealAddress() = 0;
	
	
	virtual int FUNCTION_CALL_MODE Reserved8() = 0;
	virtual int FUNCTION_CALL_MODE Reserved9() = 0;
	
	//20130527 xuxp 新增GetSelfAddress函數，來獲取自己本地的IP和端口
	/**
	* 獲取ip地址和端口
	* @return 獲取ip地址和端口，格式：ip地址+端口
	*/
	virtual const char* FUNCTION_CALL_MODE GetSelfAddress() = 0;

	//20130529 xuxp 新增GetSelfMac函數，來獲取自己本地使用的網卡MAC
	/**
	* 獲取MAC地址
	* @return MAC的地址字符串格式，類似“D067E5556D83”,中間沒有分隔符
	*/
	virtual const char* FUNCTION_CALL_MODE GetSelfMac() = 0;
	
	//20130609 xuxp 新增訂閱發布接口
	///////////////////////////////////下面增加訂閱發布的接口///////////////////////////////////////

	/**
    * 創建一個訂閱者
    * @param lpCallback 回調接口
    * @param SubScribeName 訂閱者名字，多訂閱者的名字必須不一樣，不可以相同.最大長度32個字節
    * @param iTimeOut 超時時間
    * @param iInitRecvQLen 初始化接收隊列的長度
    * @param iStepRecvQLen 接受隊列的擴展步長
    * @return 返回訂閱接口實例，一個會話接口對應一個回調.
    */
    virtual CSubscribeInterface* FUNCTION_CALL_MODE NewSubscriber(CSubCallbackInterface *lpCallback,char* SubScribeName,int iTimeOut,
        int iInitRecvQLen=INIT_RECVQ_LEN,int iStepRecvQLen=STEP_RECVQ_LEN) = 0;
    
   /**
    * 獲取發布者
    * @param PublishName 發布者業務名
    * @param msgCount 本地緩存消息的個數
	  * @param iTimeOut 初始化的時候的超時時間
    * @param bResetNo 是否重置序
    * @return 返回發送接口實例，返回對應的指針
    */
    //virtual CPublishInterface* FUNCTION_CALL_MODE GetPublisher(int msgCount,int iTimeOut,bool bResetNo = false) = 0;
    virtual CPublishInterface* FUNCTION_CALL_MODE NewPublisher(char* PublishName,int msgCount,int iTimeOut,bool bResetNo = false) = 0;
    
  


   /**
    * 獲取服務端的所有主題信息
    * @param byForce 是否強制從後台獲取
    * @param iTimeOut 超時時間
    * @return 成功就返回所有主題信息
    * @note 解包器外面需要調用release接口進行釋放.
    * @note packer返回字段
    * - TopicName
    * - TopicNo
    * - ReliableLevel
    * - IssuePriority
    * - MsgLifetime
    * - Stutas 
    * - TickStrategy 
    * - BusinessVerify
    * - Local
    * - FilterField1 
    * - FilterField2 
    * - FilterField3 
    * - FilterField4 
    * - FilterField5 
    * - FilterField6
	* - FilterField7 
	* - FilterField8 
	* - FilterField9 
	* - FilterField10 
	* - FilterField11 
	* - FilterField12
	* - FilterField13 
	* - FilterField14 
	* - FilterField15 
	* - FilterField16
    * - SubscribeStr 
    * - PublishStr
	
    */
    virtual IF2UnPacker* FUNCTION_CALL_MODE GetTopic(bool byForce,int iTimeOut) = 0;
	
	/**
	* 獲取訂閱發布的最後錯誤
	*/
	virtual const char* FUNCTION_CALL_MODE GetMCLastError() = 0;
	////////////////////////////////////////////////////////////////////////////////

	//20130624 xuxp 連接接口增加下面三個接口，用來作為服務端的接口，客戶端開發也推薦使用
	///////////////////////////////////新的一套操作接口///////////////////////////////////////
	/**
	* 初始化連接對象
	* @param lpCallback 回調對象
	* @return 返回0表示成功，否則表示失敗，通過調用GetErrorMsg可以獲取詳細錯誤信息
    * 如果應用不需要任何回調方法，則可向該方法傳遞NULL，而不必自定義回調類和對象
	*/
	virtual int FUNCTION_CALL_MODE Create2BizMsg(CCallbackInterface *lpCallback) = 0;

	/**
    * 發送業務數據
    * @param lpMsg       業務消息接口指針
    * @param nAsy        0表示同步，否則表示異步。
    * 同步發送的包，通過調用RecvBizMsg來接收，異步發送的包，當收到應答包後，自動觸發回調函數OnReceivedBizMsg。
    * @return 返回發送句柄（正數），否則表示失敗，通過調用GetErrorMsg可以獲取詳細錯誤信息
    */
	virtual int FUNCTION_CALL_MODE SendBizMsg(IBizMessage* lpMsg,int nAsy = 0) = 0;
	
	/**
	* 接收數據
	* @param hSend     發送句柄
	* @param lpMsg	   收到業務消息指針的地址
	* @param uiTimeout 超時時間，單位毫秒，0表示不等待
	* @param uiFlag    接收選項，0表示接收超時後仍可繼續調用Receive來接收，
    *                  JustRemoveHandle表示當接收超時時，把packet_id刪除（以後再收到，則會以異步的方式收到）
	* @return 返回0表示成功，否則表示失敗，通過調用GetErrorMsg可以獲取詳細錯誤信息
	* 注意！外部指針所指向的IBizMessage的內存由SDK內部管理，外部切勿釋放！
	* 注意！lpMsg對應的消息指針是臨時的，上層不可以緩存指針，再次調用這個連接的RecvBizMsg，這個指針指向的內容就會改變
	* 如果要拷貝，需要調用IBizMessage的GetBuff方法，二進制拷貝出去，其他線程需要再調用SetBuff；
	*/
	virtual int FUNCTION_CALL_MODE RecvBizMsg(int hSend, IBizMessage** lpMsg, unsigned uiTimeout = 1000, unsigned uiFlag = 0) = 0;
	
  virtual int FUNCTION_CALL_MODE Reserved10() = 0;
	virtual int FUNCTION_CALL_MODE Reserved11() = 0;
	
	//20140618 majc 增加獲取最後一個應答錯誤的詳細信息接口
	/**
	* 取返回錯誤消息的詳細信息
	* @param bAsyError 0表示同步(默認)，否則表示異步。
	* @return 返回詳細錯誤信息
	* @note 返回信息格式：packType:xxx;funtionId:xxx;branchNo:zxx;systemNo:xxx;subSystemNO:xxx;packId:xxx;routerInfo:xxx,xxx,xxx,xxx,xxx,xxx;sendPath:xxx,xxx,xxx,xxx,xxx,xxx;returnCode:xxx;errorNo:xxx;errorInfo:xxx
	* packType-包類型
	* funtionId-功能號
	* branchNo-分支號
	* systemNo-系統號
	* subSystemNO-子系統號
	* packId-包序號
	* routerInfo-目標路由
	* sendPath-發送者路由
	* returnCode-返回錯誤碼
	* errorNo-錯誤號
	* errorInfo-錯誤信息
	* 調用說明：1：同步調用時，方法Receive返回的lppData解析ESBMessage後RetuenCode不為0時調用；方法RecvBiz、RecvBizEx是在返回值為1、2、3、4時調用；方法RecvBizMsg是在GetReturnCode()不為0時調用
	*           2：異步調用時，回調OnReceived的lpData解析ESBMessage後RetuenCode不為0時調用；回調OnReceivedBiz、OnReceivedBizEx的nResult為1、2、3、4時調用；回調OnReceivedBizMsg的lpMsg在GetReturnCode()不為0時調用
	*/
	virtual const char * FUNCTION_CALL_MODE GetLastAnsError(bool bAsyError = 0) = 0;
	////////////////////////////////////////////////////////////////////////////////
	
};

extern "C"
{
    /**
	* 獲取T2_SDK的版本號
	* @return 當前T2_SDK的版本號
	* 譬如：版本為0x10000002表示1.0.0.2
	*/
    int FUNCTION_CALL_MODE GetVersionInfo();

    /**
	* 獲取一個實現CConfigInterface接口的類的指針
	* @return 實現CConfigInterface接口的類的指針
	*/
CConfigInterface* FUNCTION_CALL_MODE NewConfig();

    /**
	* 獲取一個實現CConnectionInterface接口的類的指針
	* @param CConfigInterface 實現CConfigInterface接口的類的指針
	* @return實現CConnectionInterface接口的類的指針
	*/
CConnectionInterface* FUNCTION_CALL_MODE NewConnection(CConfigInterface *lpConfig);

/**
* @param int iVersion 業務包格式版本(取值:1 字串版,其他值 0x20版)
* 推薦使用0x20版
* @return IPacker * 打包器接口指針
*/
IF2Packer * FUNCTION_CALL_MODE NewPacker(int iVersion);

/**
* @param void * lpBuffer 要解包的數據（不含AR通信包頭）
* @param unsigned int iLen 數據長度
* @return IUnPacker * 版本2結果集操作接口指針
*/
IF2UnPacker * FUNCTION_CALL_MODE NewUnPacker(void * lpBuffer, unsigned int iLen);

/**
* @param void * lpBuffer 要解包的數據（不含AR通信包頭）
* @param unsigned int iLen 數據長度
* @return IUnPacker * 版本1結果集操作接口指針
*/
IF2UnPacker * FUNCTION_CALL_MODE NewUnPackerV1(void * lpBuffer, unsigned int iLen);

/**
* @param void * lpBuffer 要解包的數據（不含AR通信包頭）
* @return 1表示版本1的結果集數據，0x21~0x2F 版本2的結果集數據
*/
int FUNCTION_CALL_MODE GetPackVersion(const void *lpBuffer);
/**
* @param char *EncodePass 傳出的散列結果，字符串，長度不超過16（包括'\0'）
* @param const char* Password 傳入的待散列的密碼
* @param int nReserve 保留參數，採用默認值
* @return char * 傳出的散列結果地址，同EncodePass
*/
char * FUNCTION_CALL_MODE Encode(char *EncodePass, const char* Password, int nReserve = 0 );


/**
*利用一部提供的加密函數來對密碼進行加密
* @param const char* pIn 傳入的待加密的字符串
* @param const char* pOut 輸出參數，長度和傳入的字符串大小一樣，由外部申請
* @return 
*/
int FUNCTION_CALL_MODE EncodeEx(const char *pIn, char *pOut);


/**
* 構造一個過濾器接口指針
* @return 返回NULL表示失敗.
*/
CFilterInterface* FUNCTION_CALL_MODE NewFilter();
    
/**
* 構造一個訂閱屬性接口指針
* @return 返回NULL表示失敗.
*/
CSubscribeParamInterface* FUNCTION_CALL_MODE NewSubscribeParam();

//20130625 xuxp 構造業務消息
IBizMessage* FUNCTION_CALL_MODE NewBizMessage();

}

#endif
