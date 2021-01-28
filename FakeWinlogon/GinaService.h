#include "ServiceBase.h"
#include "FileLog.h"

class GinaService : public Utils::CServiceBase {
public:
	GinaService();
	virtual ~GinaService();

protected:
	virtual void OnStop();
	virtual void OnContinue();
	virtual void RunService();
	void RunGina();

private:
	bool stop_;
	bool run_gina_;
	Utils::CFileLog log_;
};
