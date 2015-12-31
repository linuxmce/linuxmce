
#include "api.h"

#ifndef __HEADER_INCLUDED_LOG__
#define __HEADER_INCLUDED_LOG__

#ifdef _DEBUG

namespace PlutoITunesMediaConnector {
	namespace Broadcaster {
	
class PITMCB_API Log {
	private:
		Log();
	public:
		~Log();
		static Log& getInstance();
		void append(const char* const message, const char* const file, unsigned int line);
	private:
		Log(const Log&);
		void operator =(const Log&);
		static Log _instance;
	};

} }

#	define REGISTER_CUSTOM_MESSAGE(message, file, line) \
		do { \
		PlutoITunesMediaConnector::Broadcaster::Log::getInstance().append(message, file, line); \
			break; \
		} while ("foo" + 0);

#	define REGISTER_COM_ERROR(hresult, file, line)
		/*do { \
			std::ostringstream ss; \
			ss << "Current COM-errors:"; \
			CComPtr<IReposErrorQueueHandler> errh; \
			repository->QueryInterface(IID_IReposErrorQueueHandler, (void**)&errh); \
			CComPtr<IRepositoryErrorQueue> errq; \
			errh->GetErrorQueue(&errq); \
			const unsigned long errqcnt = errq->Count(); \
			for (unsigned long i = 0; i < errqcnt; i++) { \
				REPOSERROR rerr; \
				errq->Item(i + 1, &rerr); \
				ss << " '" << rerr.rcMsg << "';"; \
			} \
			Log::getInstance().append(ss.str().c_str(), file, line); \
			break; \
		} while ("foo" + 0);*/


#	define REGISTER_RUNTIME_POINT(method, file, line) REGISTER_CUSTOM_MESSAGE(method, file, line)

#else // _DEBUG
#	define REGISTER_CUSTOM_MESSAGE(message, file, line)
#	define REGISTER_COM_ERROR(hresult, file, line)
#	define REGISTER_RUNTIME_POINT(method, file, line)
#endif // _DEBUG

#endif // __HEADER_INCLUDED_LOG__
