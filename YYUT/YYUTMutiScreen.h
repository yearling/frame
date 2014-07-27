
#ifndef __YYTOOLMUTISCREEN_H__
#define __YYTOOLMUTISCREEN_H__

#include <windows.h>
#include <vector>
#include <string>
using namespace  std;
#ifdef UNICODE
#define YYSetConsole YYSetConsoleW
#else
#define YYSetConsole YYSetConSoleA
#endif

namespace YYUT
{
	class ScreenInfo
	{
	public:
		long lPelsWidth;
		long lPelsHeight;
		long lBitPerPel;
		ScreenInfo():lPelsWidth(0),lPelsHeight(lPelsWidth),lBitPerPel(0)
		   {
			  

		   }
		ScreenInfo(long lWidth,long lHeight,long BitPerPel):lPelsWidth(lWidth),lPelsHeight(lHeight),lBitPerPel(BitPerPel){};
	};
	struct HwndANDPosition
	{
		HWND hwnd;
		RECT rc;
		HwndANDPosition():hwnd(NULL)
		{
			rc.bottom=0;
			rc.left=0;
			rc.right=0;
			rc.top=0;
		}
	};
	class CMutiScreen
	{
	public:
		CMutiScreen();
		static int m_nNumber;//��Ļ����
		static BOOL __stdcall EnumProc(HMONITOR hMonitor,HDC hdcMonitor,LPRECT lprcMonitor,LPARAM dwData);//�ص�����
		static HMONITOR m_hPrimior;//����Ļ���
		static HMONITOR m_hOther;//����Ļ���
#ifdef UNICODE
		std::wstring m_strPrimiorName;
		std::wstring m_strOtherName;
#else
		std::string m_strPrimiorName;
		std::string m_strOtherName;
#endif
		enum OtherPosition{SCREEN_LEFT=0,SCREEN_RIGHT,SCREEN_TOP,SCREEN_DOWN};//����Ļ���������Ļ��λ��
		OtherPosition GetOtherPosition();//�õ�����Ļ���������Ļ��λ��
		RECT GetRect(HMONITOR hMonitor);//�õ����ڵ��������ꡣ
		std::vector<ScreenInfo> m_vPrimerMonitor;//����Ļ��Ӳ������
		std::vector<ScreenInfo> m_vOtherMointor;//����Ļ��Ӳ������
		std::vector<HwndANDPosition> m_hpVSWindows;
		ScreenInfo GetDefaultScreenInfo(bool PrimiorMonitor);//�õ�Ĭ�ϵ���ʾ���ã�trueΪ���ص�������Ļ
		virtual bool Init();
		virtual void Restore();
		virtual ~CMutiScreen();
	private:
#ifdef UNICODE
		void InitScreenInfo(HMONITOR hMonitor,std::vector<ScreenInfo> &vec,std::wstring &strName);//��ָ������Ļ��������������
#else
		void InitScreenInfo(HMONITOR hMonitor,std::vector<ScreenInfo> &vec,std::string &strName);//��ָ������Ļ��������������
#endif
		HWND m_hCurrentMainHWND;
		HWND m_hVSMainHWND;
	};
	void YYSetConsoleA(HWND=nullptr);//������Ϊ��DebugConsole����λ��Ϊ�ڶ���Ļ�Ŀ���̨
	void YYSetConsoleW(HWND=nullptr);//������Ϊ��DebugConsole����λ��Ϊ�ڶ���Ļ�Ŀ���̨
	void YYSetConsoleA( string file_name);//�����붨�嵽ĳ�ļ���������log
	void YYSetConsoleW(wstring file_name);
	CMutiScreen* GetMutiScreen();//����ģʽ���õ�CMutiScrren
}

#endif