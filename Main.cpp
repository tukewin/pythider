#include <Windows.h>
#include <intrin.h>

#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "mh/MinHook.h"

#define UNLEN 256

using memcmp_t = int(__cdecl *)(const void *, const void *, size_t);
using memcpy_t = void *(__cdecl *)(void *, const void *, size_t);

memcmp_t o_memcmp = nullptr;
memcpy_t o_memcpy = nullptr;

bool is_valid_string(const void *ptr) {
  MEMORY_BASIC_INFORMATION mbi;

  if (VirtualQuery(ptr, &mbi, sizeof(mbi)) == sizeof(mbi)) {
    if (mbi.State == MEM_COMMIT &&
        (mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY))) {
      const char *str = reinterpret_cast<const char *>(ptr);

      for (size_t i = 0; str[i] != '\0'; ++i) {
        if (str[i] < 32 || str[i] > 126) {
          return false;
        }
      }

      if (strlen(str) < 2) {
        return false;
      }

      return true;
    }
  }

  return false;
}

int __cdecl hooked_memcmp(const void *ptr1, const void *ptr2, size_t num) {
  if (is_valid_string(ptr1) || is_valid_string(ptr2)) {
    auto shit1 = reinterpret_cast<const char *>(ptr1);
    auto shit2 = reinterpret_cast<const char *>(ptr2);

    if (strstr(shit1, ("User Is Blacklisted."))) {
      MessageBoxA(0, ("spoofed blacklist"), 0, 0);
      memcpy((void *)shit1, ("pytgavno"), 6);
    } else {
      std::cout << "shit1: " << shit1 << std::endl;
      std::cout << "shit2: " << shit2 << std::endl;
    }
  }

  return o_memcmp(ptr1, ptr2, num);
}

void *__cdecl hooked_memcpy(void *dest, const void *src, size_t count) {
  if (is_valid_string(src)) {
    auto shit1 = reinterpret_cast<const char *>(src);
    auto shit2 = reinterpret_cast<const char *>(dest);

    TCHAR username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    GetUserName(username, &username_len);
    std::wstring ws(username);
    std::string pc_username(ws.begin(), ws.end());

    if (strstr(shit1, pc_username.c_str())) {
      MessageBoxA(0, ("identified handler"), 0, 0);
      memcpy((void *)src, ("pytgavno"), 6);
    } else {
      std::cout << "src: " << shit1 << std::endl;
      std::cout << "dest: " << shit2 << std::endl;
    }
  }

  return o_memcpy(dest, src, count);
}

void init() {
  LoadLibraryA("user32.dll");
  LoadLibraryA("kernel32.dll");
  MH_Initialize();

  if (MH_CreateHook(&memcmp, &hooked_memcmp,
                    reinterpret_cast<LPVOID *>(&o_memcmp)) != MH_OK) {
    printf("fail #1 \n");
    return;
  }

  if (MH_CreateHook(&memcpy, &hooked_memcpy,
                    reinterpret_cast<LPVOID *>(&o_memcpy)) != MH_OK) {
    printf("fail #2 \n");
    return;
  }

  MH_EnableHook(MH_ALL_HOOKS);

  std::cout << "all good!" << std::endl;

  while (1 == 1) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

BOOL WINAPI DllMain(HMODULE hInstance, DWORD fdwReason, void *reserved) {
  if (fdwReason == DLL_PROCESS_ATTACH) {
    DisableThreadLibraryCalls(hInstance);
    CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(&init),
                 nullptr, 0, nullptr);
  } else if (fdwReason == DLL_PROCESS_DETACH) {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
  }
  return TRUE;
}