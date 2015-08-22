// MainDlg.h : Declaration of the CMainDlg

#ifndef _SMARTCOM_PROXY2MAINDLG_INCLUDED
#define _SMARTCOM_PROXY2MAINDLG_INCLUDED

#include "resource.h"       // main symbols

#include "shared/cpputils/src/cpputils.hpp"
#include <atlhost.h>


#define WM_TRAY_ICON_MESSAGE (WM_USER+1997)

namespace SmartComHelperLib {

	class FileEntry {

	public:
		FileEntry():
				fs_m(NULL)
				{
				}

				~FileEntry()
				{
					deinit();
				}

				void init(CppUtils::String const& fileName)
				{
					deinit();

					fileName_m = fileName;
					fs_m = new fstream();
					fs_m->open(fileName.c_str(), ios::in | ios::out | ios::trunc);

				}

				void deinit()
				{
					if (fs_m != NULL) {
						if (fs_m->is_open()) {
							fs_m->flush();
							fs_m->close();
						}

						delete fs_m;

						fs_m = NULL;
						fileName_m = "";
					}
				}

				inline fstream& getStream()
				{
					return *fs_m;
				}

				
				inline bool const isOpen() const
				{
					return (fs_m != NULL && fs_m->is_open());
				}

				inline CppUtils::String const& getFileName() const
				{
					return fileName_m;
				}

	private:
		CppUtils::String fileName_m;

		fstream* fs_m;

	};

	// CMainDlg
	class CMainSmartComEngine;
	class CMainDlg : 	public CAxDialogImpl<CMainDlg>
	{
	public:
		CMainDlg(CMainSmartComEngine& mainEngine);
		~CMainDlg();


		enum { IDD = IDD_MAINDLG };

		BEGIN_MSG_MAP(CMainDlg)
			MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
			MESSAGE_HANDLER(WM_CLOSE, OnClose );
			MESSAGE_HANDLER(WM_TRAY_ICON_MESSAGE, OnIconMesssage );
			MESSAGE_HANDLER(WM_TIMER, OnTimer );
			MESSAGE_HANDLER(WM_DESTROY, OnDestroy); 
			 

			COMMAND_HANDLER(IDC_BUTTON_QUERY_SYMB, BN_CLICKED, OnBnClickedButtonQuerySymb)
			COMMAND_HANDLER(IDC_BUTTON_GET_PORTFOLIO, BN_CLICKED, OnBnClickedButtonGetPortfolio)
			COMMAND_HANDLER(IDC_GET_STATUS, BN_CLICKED, OnBnClickedButtonStatus)
			COMMAND_HANDLER(IDC_CANCEL_HIST, BN_CLICKED, OnBnClickedButtonCancelHistory)
			COMMAND_HANDLER(IDC_DISCONNECT, BN_CLICKED, OnBnClickedDisConnect)
			COMMAND_HANDLER(IDC_SIMULATE, BN_CLICKED, OnBnClickedSimulate);

			
			CHAIN_MSG_MAP(CAxDialogImpl<CMainDlg>)
		END_MSG_MAP()

		// Handler prototypes:
		//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
		//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);


		void addLogEntry(CppUtils::String const& logMsg);

			void initialize(char const* loginId);

			void setUpInitialData(CppUtils::String const&clientVerString, CppUtils::String const&srvVerString);

			// - GetSymbols
			void clearSymbolList();

			void addSymbolEntry(
				long const& row, 
				long const& nrows, 
				char const* symbol, 
				char const* short_name, 
				char const*  long_name, 
				char const*  type, 
				long const& decimals, 
				long const& lot_size, 
				double const& punkt, 
				double const&  step, 
				char const*  sec_ext_id, 
				char const*  sec_exch_name, 
				double const& expiry_date, 
				double const& days_before_expiry
				);

			void addSymbolHeader(
				char const* row_c, 
				char const* nrows_c, 
				char const* symbol_c, 
				char const* short_name_c, 
				char const*  long_name_c, 
				char const*  type_c, 
				char const* decimals_c, 
				char const* lot_size_c, 
				char const* punkt_c, 
				char const*  step_c, 
				char const*  sec_ext_id_c, 
				char const*  sec_exch_name_c, 
				char const* expiry_date_c, 
				char const* days_before_expiry_c
				);

			//

			// GetPrortfolioList 
			void clearPortfolioList();

			void addPortfolioRow(long row, long nrows, char const* portfolioName, char const* portfolioExch);

			//

			void setConnectionStatus(bool const is_connected);

			//void navigateToUrl(CppUtils::String const& url);

			static CppUtils::String createTempHtml();

			void openTempHtml(CppUtils::String const& file);

			void mesageBox(char const* message);

			void mesageBox(CppUtils::String const& message);

			void showIcon();

			void hideIcon();



	private:

		void generateSimulatingTicks_Bars();

		CMainSmartComEngine& mainEngine_m;  

		FileEntry symbolFileEntry_m;

		FileEntry portfolioEntry_m;

		CppUtils::String serverVersionString_m;

		CppUtils::String clientVersionString_m;

		bool simulatingTicks_m;

		int simulatingTicksCounter_m;


	public:
		LRESULT OnBnClickedButtonQuerySymb(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		LRESULT OnBnClickedButtonGetPortfolio(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		LRESULT OnBnClickedButtonStatus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		LRESULT OnBnClickedButtonCancelHistory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		LRESULT OnBnClickedDisConnect(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		LRESULT OnBnClickedSimulate(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
		
		LRESULT OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

		LRESULT OnIconMesssage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);


		
	};

};

#endif

