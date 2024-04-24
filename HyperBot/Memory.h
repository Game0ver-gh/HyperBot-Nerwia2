#pragma once

class Memory
{
public:
	Memory();
	static DWORD FindSignature(const char* signature, int extra_opcode = 0, int relative = 0, const char* module = MODULE_NAME);
	static std::vector<DWORD> FindSignatures(const char* signature, const char* module = MODULE_NAME);
	DWORD GetAddress(const std::string& name);
	DWORD* GetPointer(const std::string& name);

	template<unsigned int IIdx, typename TRet, typename ... TArgs>
	static auto CallVirtualFn(void* thisptr, TArgs ... argList) -> TRet
	{
		using Fn = TRet(__thiscall*)(void*, decltype(argList)...);
		return (*static_cast<Fn**>(thisptr))[IIdx](thisptr, argList...);
	}

private:
	bool MapSignatures();
	uint32_t sig_total, sig_found;
	std::vector<std::string> sig_failed;
	std::map<std::string, DWORD> address;
	std::map<std::string, DWORD*> pointer;
};

