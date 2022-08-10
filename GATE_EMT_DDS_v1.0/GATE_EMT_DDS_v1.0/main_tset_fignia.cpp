#include <LoggerScada.hpp>


enum class SecurityMode
{
	None,
	Sign,
	SignEndEncrypt
};

std::ostream& operator<<(std::ostream& os, const SecurityMode& mode) {

	std::string str;
	switch (mode)
	{
	case SecurityMode::None:
		str = "None";
		break;
	case SecurityMode::Sign:
		str = "Sign";
		break;
	case SecurityMode::SignEndEncrypt:
		str = "SignEndEncrypt";
		break;
	default:
		break;
	}

	return os << str;
}

int main()
{
	std::shared_ptr<LoggerSpaceScada::ILoggerScada> log = LoggerSpaceScada::GetLoggerScada(LoggerSpaceScada::TypeLogger::SPDLOG);
	log->SetLevel(LoggerSpaceScada::LevelLog::Debug);
	log->Info("Start {} {}", 1, 2);

	SecurityMode sec = SecurityMode::Sign;
	log->Debug("Test");
	log->Debug("TRES {}", sec);
	return 0;
}