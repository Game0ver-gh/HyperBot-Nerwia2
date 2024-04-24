#pragma once

//#define DEV_MODE

#ifdef DEV_MODE
#define DEV_PRINTF(fmt, ...) G::console->WriteDebug("%s(): " fmt "\n", __func__, ##__VA_ARGS__)
#else
#define DEV_PRINTF(fmt, ...)
#endif

class HookManager
{
public:
#define THISCALL_DEF(ret_type, fn_name, ...)	\
	using t##fn_name = ret_type(__thiscall*)(void* pThis, __VA_ARGS__);	\
	static inline t##fn_name o##fn_name = nullptr;	\
	static ret_type __fastcall fn_name(void* pThis, void* edx, __VA_ARGS__);

#define FASTCALL_DEF(ret_type, fn_name, ...)	\
	using t##fn_name = ret_type(__fastcall*)(void* pThis, __VA_ARGS__);	\
	static inline t##fn_name o##fn_name = nullptr;	\
	static ret_type __fastcall fn_name(void* pThis, __VA_ARGS__);

#define STDCALL_DEF(ret_type, fn_name, ...)	\
	using t##fn_name = ret_type(__stdcall*)(void* pThis, __VA_ARGS__);	\
	static inline t##fn_name o##fn_name = nullptr;	\
	static ret_type __stdcall fn_name(void* pThis, __VA_ARGS__);
	
	explicit HookManager();
	~HookManager();

	THISCALL_DEF(bool, RecvWhisperPacket);
	THISCALL_DEF(bool, __SendHack, const char* );
	THISCALL_DEF(int, IncreaseExternalForce, const Vector&, float);
	FASTCALL_DEF(void, BlockMovement);
	THISCALL_DEF(int, TestActorCollision, int);
	THISCALL_DEF(bool, SendUseSkillPacket, DWORD, DWORD);
	THISCALL_DEF(void, UpdateKeyboard);
	STDCALL_DEF(HRESULT, EndScene);
	STDCALL_DEF(HRESULT, Reset, void*);
	THISCALL_DEF(int, ProcessTerrainCollision);
	THISCALL_DEF(void, __ShowMapName, LONG, LONG);
	THISCALL_DEF(void, OnScriptEventStart, int, int);
	THISCALL_DEF(bool, SendAttackPacket, UINT, VID);
	
	THISCALL_DEF(void, RenderGame);
	THISCALL_DEF(void, UpdateGame);
	THISCALL_DEF(void, UIRender);

	THISCALL_DEF(bool, Send, int, char*);
	THISCALL_DEF(bool, Recv, int, char*);

	THISCALL_DEF(bool, RecvDamageInfoPacket);

	static LRESULT __stdcall WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#ifdef DEV_MODE
	
	THISCALL_DEF(bool, SendOnClickPacket, VID);
	THISCALL_DEF(bool, SendScriptAnswerPacket, BYTE);//0, 1, 2, 3, ... - buttons ids from top to bottom
	THISCALL_DEF(bool, SendGiveItemPacket, VID, TItemPos, BYTE);
	THISCALL_DEF(bool, SendShootPacket, UINT);
	THISCALL_DEF(bool, SendTargetPacket, VID);
	THISCALL_DEF(bool, SendAddFlyTargetingPacket, VID, Vector&);
	THISCALL_DEF(bool, SendFlyTargetingPacket, VID, Vector&);
	THISCALL_DEF(bool, SendScriptButtonPacket, int);
	THISCALL_DEF(bool, SendRefinePacket, BYTE, BYTE);
	THISCALL_DEF(bool, SendSyncPositionPacket, VID, DWORD, DWORD);
	THISCALL_DEF(bool, SendCharacterStatePacket, const Vector&, float, UINT, UINT);
#endif
	
private:
#define CREATE_HOOK(fn_name)	\
	err = MH_CreateHook((void*)G::memory->GetAddress(#fn_name), &fn_name, reinterpret_cast<LPVOID*>(&o##fn_name));	\
	if (err != MH_OK) G::console->WriteError("Hooking %s() failed with error %s!\n", #fn_name, MH_StatusToString(err));	\
	else created_hooks++;

#define CREATE_HOOK_SIG(fn_name, sig)	\
	err = MH_CreateHook((void*)G::memory->FindSignature(sig), &fn_name, reinterpret_cast<LPVOID*>(&o##fn_name));	\
	if (err != MH_OK) G::console->WriteError("Hooking %s() failed with error %s!\n", #fn_name, MH_StatusToString(err));	\
	else created_hooks++;

#define CREATE_HOOK_SIGS(fn_name, sig, addr_idx)	\
	err = MH_CreateHook((void*)G::memory->FindSignatures(sig).at(addr_idx), &fn_name, reinterpret_cast<LPVOID*>(&o##fn_name));	\
	if (err != MH_OK) G::console->WriteError("Hooking %s() failed with error %s!\n", #fn_name, MH_StatusToString(err));	\
	else created_hooks++;

#define CREATE_HOOK_ADDR(fn_name, addr)	\
	err = MH_CreateHook((void*)(addr), &fn_name, reinterpret_cast<LPVOID*>(&o##fn_name));	\
	if (err != MH_OK) G::console->WriteError("Hooking %s() failed with error %s!\n", #fn_name, MH_StatusToString(err));	\
	else created_hooks++;

	static inline HWND hWnd{};
	static inline WNDPROC oWndproc{};
	int created_hooks;
};

class Console
{
public:
	explicit Console();
	~Console();

	void WriteLine()
	{
		printf_s("\n");
	}

	//%s
	template <typename... T> void Write(const std::string& format, T... args)
	{
		char buff[2024];
		sprintf_s(buff, sizeof(buff), format.c_str(), args...);
		printf_s("%s", buff);
	}

	//[>] %s [CYAN]
	template <typename... T> void WriteCmd(const std::string& format, T ...args)
	{
		auto col = this->color;
		SetColor(TextColor::CYAN);
		char buff[2024];
		sprintf_s(buff, sizeof(buff), format.c_str(), args...);
		printf_s("[>] %s", buff);
		SetColor(col);
	}

	//[+] %s
	template <typename... T> void WriteInfo(const std::string& format, T... args)
	{
		char buff[2024];
		sprintf_s(buff, sizeof(buff), format.c_str(), args...);
		printf_s("[+] %s", buff);
		SetColor(TextColor::WHITE);
	}

	//[!] %s [RED]
	template <typename... T> void WriteError(const std::string& format, T... args)
	{
		auto col = this->color;
		SetColor(TextColor::RED);
		char buff[2024];
		sprintf_s(buff, sizeof(buff), format.c_str(), args...);
		printf_s("[!] %s", buff);
		SetColor(col);
	}

	//[&] %s [PINK]
	template <typename... T> void WriteDebug(const std::string& format, T... args)
	{
		auto col = this->color;
		SetColor(TextColor::PINK);
		char buff[2024];
		sprintf_s(buff, sizeof(buff), format.c_str(), args...);
		printf_s("[&] %s", buff);
		SetColor(col);
	}

	enum class TextColor
	{
		GREEN = 10,
		CYAN,
		RED,
		PINK,
		YELLOW,
		WHITE,
	};

	void SetColor(TextColor color);
	 
	void WriteMultiColor(const std::vector<std::pair<std::string, TextColor>>& text)
	{
		for (const auto& [fmt, col] : text)
		{
			SetColor(col);
			printf_s(fmt.c_str());
		}
		SetColor(TextColor::WHITE);
	}
	
	using MultiColor = std::vector<std::pair<std::string, Console::TextColor>>;

private:

	TextColor color;
};
