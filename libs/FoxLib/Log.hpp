/*Logs data to log.txt.
  Note: this performs no error checking on write
  because it logs errors so if its broken you're screwed
  log flushes on each write because we want to log pre-crash errors*/
struct textLog {
	HANDLE logFile;
	
	//Opens the log file
	bool init() {
		this->logFile = CreateFileA("log.txt",
		                            GENERIC_WRITE,
		                            FILE_SHARE_READ,
		                            nullptr,
		                            CREATE_ALWAYS,
		                            0,
		                            nullptr);
		return this->logFile != INVALID_HANDLE_VALUE;
	}
	void flush() {
		BOOL worked = FlushFileBuffers(this->logFile);
		assert(worked);
	}
	/*There's no free because we always flush so no need to close
	  since this log should last the lifetime of the program.*/
	
	void writeStr(constStr c, bool printNL = false) {
		if (logFile != nullptr) {
			DWORD bytesWritten;
			WriteFile(logFile,
					  c.b,
			          (DWORD)c.len,
					  &bytesWritten,
					  nullptr);
		}
		flush();
		if (printNL) {
			this->writeStr("\n", false);
		}
	}
	void writeUint(uint64 n, bool printNL = false) {
		char nBuf[maxUint64StrLen];
		this->writeStr(str().fromUint(n, nBuf).constify(), printNL);
	}
	void writeInt(int64 n, bool printNL = false) {
		char nBuf[maxInt64StrLen];
		this->writeStr(str().fromInt(n, nBuf).constify(), printNL);
	}
	void writeFloat(float32 n, bool printNL = false) {
		char nBuf[maxFloat32StrLen];
		this->writeStr(str().fromFloat(n, nBuf).constify(), printNL);
	}
};
