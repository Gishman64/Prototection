// lab5.cpp: определяет точку входа для консольного приложения.
//
#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include<stdio.h>
#include<windows.h>
#include<AccCtrl.h>
#include<Aclapi.h>
#include<tchar.h>

#ifdef _DEBUG
#include<conio.h>
#endif

void error(const char *error) {
	printf("%s\n", error);

#ifdef _DEBUG
	 _getch();
#endif

	exit(1);
}

int main(int argc, char* argv[]) {
	EXPLICIT_ACCESS ea[2];
	ZeroMemory(&ea, 2 * sizeof(EXPLICIT_ACCESS));

	//получениеидентификаторагруппы Everyone
	SID_IDENTIFIER_AUTHORITY SIDAuthLocal = SECURITY_CREATOR_SID_AUTHORITY;
	PSID pLocalUserSID = NULL;

	if(!AllocateAndInitializeSid(&SIDAuthLocal, 1, SECURITY_CREATOR_OWNER_RID, 0, 0, 0, 0, 0, 0, 0, &pLocalUserSID)) {
		error("AllocateAndInitializeSid Error");
	}

	ea[0].grfAccessPermissions = KEY_READ;
	ea[0].grfAccessMode = SET_ACCESS;
	ea[0].grfInheritance = NO_INHERITANCE;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[0].Trustee.ptstrName = (LPTSTR) pLocalUserSID;

	//получениеидентификаторагруппы Admin
	SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
	PSID pAdminSID = NULL;

	if(!AllocateAndInitializeSid(&SIDAuthNT, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pAdminSID)) {
		FreeSid(pLocalUserSID);
		error("AllocateAndInitializeSid Error");
	}

	ea[1].grfAccessPermissions = KEY_ALL_ACCESS;
	ea[1].grfAccessMode = SET_ACCESS;
	ea[1].grfInheritance = NO_INHERITANCE;
	ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[1].Trustee.ptstrName = (LPTSTR) pAdminSID;

	PACL pACL = NULL;
	DWORD dwRes = SetEntriesInAcl(2, ea, NULL, &pACL);
	if(dwRes != ERROR_SUCCESS) {
		FreeSid(pLocalUserSID);
		FreeSid(pAdminSID);
		error("SetEntriesInAcl Error");
	}

	PSECURITY_DESCRIPTOR pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
	if(pSD == NULL) {
		FreeSid(pLocalUserSID);
		FreeSid(pAdminSID);
		LocalFree(pACL);
		error("LocalAlloc Error");
	}

	if(!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION)) {
		FreeSid(pLocalUserSID);
		FreeSid(pAdminSID);
		LocalFree(pACL);
		LocalFree(pSD);
		error("InitializeSecurityDescriptor Error");
	}

	if(!SetSecurityDescriptorDacl(pSD, TRUE, pACL, FALSE)) {
		FreeSid(pLocalUserSID);
		FreeSid(pAdminSID);
		LocalFree(pACL);
		LocalFree(pSD);
		error("SetSecurityDescriptorDacl Error");
	}

	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = pSD;
	sa.bInheritHandle = FALSE;

	HKEY hkSub;
	DWORD dwDisposition;

	LONG lRes = RegCreateKeyEx(HKEY_CURRENT_USER, _T("Software\\pi"), 0, NULL, 0, KEY_READ | KEY_WRITE, &sa, &hkSub, NULL);

	printf("RegCreateKeyEx result: ");
	switch(lRes) {
		case ERROR_SUCCESS:
			printf("OK!\n");
			break;
		default:
			printf("ERROR!\n");
	}
	system("pause");

	printf("\nFinished!");
}