#include <stdio.h>
#include "WbDeD.h"
#include "core.h"
#include "calc.h"
#include "state.h"

DWORD dwRegister;
WB_IWabbitemu *g_pWabbitemu;

static HRESULT STDMETHODCALLTYPE icf_queryinterface(WB_IClassFactory *, REFIID, LPVOID *);
static ULONG STDMETHODCALLTYPE icf_addref(WB_IClassFactory *);
static ULONG STDMETHODCALLTYPE icf_release(WB_IClassFactory *);
static HRESULT STDMETHODCALLTYPE icf_createinstance(WB_IClassFactory *, IUnknown *, REFIID, LPVOID *);
static HRESULT STDMETHODCALLTYPE icf_lockserver(WB_IClassFactory *, BOOL);

static HRESULT STDMETHODCALLTYPE iw_queryinterface(WB_IWabbitemu *, REFIID, LPVOID *);
static ULONG STDMETHODCALLTYPE iw_addref(WB_IWabbitemu *);
static ULONG STDMETHODCALLTYPE iw_release(WB_IWabbitemu *);
static HRESULT STDMETHODCALLTYPE iw_showmessage(WB_IWabbitemu *);
static HRESULT STDMETHODCALLTYPE iw_getstate(WB_IWabbitemu *, ULONG, Z80State_t *);
static HRESULT STDMETHODCALLTYPE iw_setstate(WB_IWabbitemu *, ULONG, Z80State_t *);
static HRESULT STDMETHODCALLTYPE iw_keypress(WB_IWabbitemu *, ULONG, DWORD);
static HRESULT STDMETHODCALLTYPE iw_keyrelease(WB_IWabbitemu *, ULONG, DWORD);
static HRESULT STDMETHODCALLTYPE iw_memread(WB_IWabbitemu *, ULONG, WORD, LPBYTE);
static HRESULT STDMETHODCALLTYPE iw_memwrite(WB_IWabbitemu *, ULONG, WORD, BYTE);
static HRESULT STDMETHODCALLTYPE iw_drawscreen(WB_IWabbitemu *, ULONG, BYTE *);
static HRESULT STDMETHODCALLTYPE iw_setbreakpoint(WB_IWabbitemu *, ULONG, HWND, BOOL, BYTE, WORD);
static HRESULT STDMETHODCALLTYPE iw_clearbreakpoint(WB_IWabbitemu *, ULONG, BOOL, BYTE, WORD);
static HRESULT STDMETHODCALLTYPE iw_getapplist(WB_IWabbitemu *, ULONG, APPENTRY *, DWORD *);
static HRESULT STDMETHODCALLTYPE iw_getsymlist(WB_IWabbitemu *, ULONG, SYMENTRY *, DWORD *);
static HRESULT STDMETHODCALLTYPE iw_setmemstate(WB_IWabbitemu *, ULONG, MEMSTATE *);
static HRESULT STDMETHODCALLTYPE iw_getmemstate(WB_IWabbitemu *, ULONG, MEMSTATE *);
static HRESULT STDMETHODCALLTYPE iw_step(WB_IWabbitemu *, ULONG);
static HRESULT STDMETHODCALLTYPE iw_stepover(WB_IWabbitemu *, ULONG);
static HRESULT STDMETHODCALLTYPE iw_stepout(WB_IWabbitemu *, ULONG);
static HRESULT STDMETHODCALLTYPE iw_run(WB_IWabbitemu *, ULONG);

static WB_IWabbitemuVtbl iw_vtbl = {
	iw_queryinterface,
	iw_addref,
	iw_release,
	iw_run,
	iw_getstate,
	iw_setstate,
	iw_keypress,
	iw_keyrelease,
	iw_memread,
	iw_memwrite,
	iw_drawscreen,
	iw_setbreakpoint,
	iw_clearbreakpoint,
	iw_getapplist,
	iw_getsymlist,
	iw_getmemstate,
	iw_setmemstate,
	iw_step,
	iw_stepover,
	iw_stepout
};

static WB_IClassFactoryVtbl icf_vtbl = {
	icf_queryinterface,
	icf_addref,
	icf_release,
	icf_createinstance,
	icf_lockserver
};




static HRESULT STDMETHODCALLTYPE
icf_queryinterface(	WB_IClassFactory *This,
					REFIID riid,
					LPVOID *ppvObject )
{
	if (ppvObject == NULL) {
		printf("IClassFactory QueryInterface E_INVALIDARG\n");
		return E_INVALIDARG;
	}

	if (IsEqualGUID (riid, &IID_IUnknown)) {
		//printf("IClassFactory QueryInterface (IUnknown)\n");
		icf_addref(This);
		*ppvObject = This;
		return S_OK;
	} else
	if (IsEqualGUID (riid, &IID_IClassFactory)) {
		//printf("IClassFactory QueryInterface (IClassFactory)\n");
		icf_addref(This);
		*ppvObject = This;
		return S_OK;
	} else
	if (IsEqualGUID (riid, &IID_IClassFactory2)) {
		//printf("IClassFactory QueryInterface (IClassFactory2)\n");
		*ppvObject = NULL;
		return E_NOINTERFACE;
	} else {
		//printf("IClassFactory QueryInterface (Fail)\n");
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
}

static ULONG STDMETHODCALLTYPE
icf_addref(WB_IClassFactory *This)
{
	//printf("IClassFactory AddRef %ld\n", This->m_lRefCount+1);
	return InterlockedIncrement(&This->m_lRefCount);
}

static ULONG STDMETHODCALLTYPE
icf_release(WB_IClassFactory *This)
{
	LONG count = InterlockedDecrement(&This->m_lRefCount);

	if (count == 0) g_free(This);


	return count;
}

static HRESULT STDMETHODCALLTYPE
icf_createinstance(WB_IClassFactory *This, IUnknown *pUnkOuter, REFIID riid, LPVOID * ppvObject)
{
	HRESULT hr;
	LPOLESTR psz;
	StringFromCLSID(riid, &psz);

	wprintf(L"IClassFactory CreateInstance %s\n", psz);
	if (ppvObject == NULL)
		return E_INVALIDARG;

	*ppvObject = NULL;

	if (pUnkOuter != NULL)
		return CLASS_E_NOAGGREGATION;

	hr = g_pWabbitemu->unk.lpVtbl->QueryInterface((IUnknown *) g_pWabbitemu, riid, ppvObject);
	printf("Done getting interface: %ld %p\n", hr, *ppvObject);
	return hr;
}

static HRESULT STDMETHODCALLTYPE
icf_lockserver(WB_IClassFactory *This, BOOL fLock)
{
	//printf("IClassFactory LockServer\n");
	CoLockObjectExternal(g_pWabbitemu, fLock, TRUE);
	return S_OK;
}

WB_IClassFactory *WB_IClassFactory_new() {
	WB_IClassFactory *result;

	result = g_new0(WB_IClassFactory, 1);
	if (result == NULL) {
		printf("Class factory creation failed!\n");
	}
	result->icf.lpVtbl = (IClassFactoryVtbl*) &icf_vtbl;
	result->m_lRefCount = 0;

	return result;
}

static HRESULT STDMETHODCALLTYPE
iw_queryinterface(	WB_IWabbitemu *This,
					REFIID riid,
					LPVOID *ppvObject )
{

	if (ppvObject == NULL) {
		printf("IWabbitemu QueryInterface E_INVALIDARG\n");
		return E_INVALIDARG;
	}

	CLSID IID_IWabbitemu;
	CLSIDFromString((LPOLESTR) L"{13b5c004-4377-4c94-a8f9-efc1fdaeb31c}", &IID_IWabbitemu);

	LPOLESTR psz;
	StringFromCLSID(riid, &psz);

	if (IsEqualGUID (riid, &IID_IUnknown)) {
		//printf("IWabbitemu QueryInterface (IUnknown)\n");
		iw_addref(This);
		*ppvObject = This;
		return S_OK;
	} else
	if (IsEqualGUID (riid, &IID_IWabbitemu)) {
		//printf("IWabbitemu QueryInterface (IWabbitemu)\n");
		iw_addref(This);
		*ppvObject = This;
		return S_OK;
	} else {
		//wprintf(L"IWabbitemu QueryInterface %s\n", psz);
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}

}

static ULONG STDMETHODCALLTYPE
iw_addref(WB_IWabbitemu *This)
{
	//printf("IWabbitemu AddRef %ld\n", This->m_lRefCount+1);
	return InterlockedIncrement(&This->m_lRefCount);
}

static ULONG STDMETHODCALLTYPE
iw_release(WB_IWabbitemu *This)
{
	//printf("IWabbitemu Release %ld\n", This->m_lRefCount-1);
	LONG count = InterlockedDecrement(&This->m_lRefCount);

	if (count == 0)
		g_free(This);

	return count;
}

static HRESULT STDMETHODCALLTYPE
iw_showmessage(WB_IWabbitemu *This)
{
	printf("IWabbitemu ShowMessage\n");
	MessageBox(NULL, "Greetings from Wabbitemu.", "Wabbitemu", MB_OK);
	return S_OK;
}

WB_IWabbitemu *WB_IWabbitemu_new() {
	WB_IWabbitemu *result;

	result = g_new0(WB_IWabbitemu, 1);

	result->unk.lpVtbl = &iw_vtbl;
	result->m_lRefCount = 0;
	return result;
}

HRESULT RegisterClassObject() {
	HRESULT hr;

	CLSID clsid;
	hr = CLSIDFromString((LPOLESTR) L"{8cc953bc-a879-492b-ad22-a2f4dfcd0e19}", &clsid);
	if (FAILED(hr)) {
		printf("Failed to create class ID\n");
		return 1;
	}

	WB_IClassFactory *pClassFactory =  WB_IClassFactory_new();
	g_pWabbitemu = WB_IWabbitemu_new();

	hr = CoRegisterClassObject(
			&clsid,
			(IUnknown *) &pClassFactory->icf,
			//(IUnknown *) &g_pWabbitemu->unk,
			//CLSCTX_INPROC_SERVER,
			CLSCTX_LOCAL_SERVER,
			//REGCLS_SINGLEUSE,
			REGCLS_MULTI_SEPARATE,
			&dwRegister
	);

	if (FAILED(hr)) {
		printf("CoRegisterClassObject failed!\n");
		return hr;
	}
	return hr;
}

HRESULT RevokeClassObject() {
	HRESULT hr = CoRevokeClassObject(dwRegister);
	return hr;
}


static HRESULT STDMETHODCALLTYPE
iw_getstate(WB_IWabbitemu *This, ULONG slot, Z80State_t *zs)
{
	CPU_t *cpu = &calcs[slot].cpu;

	zs->AF = cpu->af;
	zs->BC = cpu->bc;
	zs->DE = cpu->de;
	zs->HL = cpu->hl;
	zs->IX = cpu->ix;
	zs->IY = cpu->iy;

	zs->AFP = cpu->afp;
	zs->BCP = cpu->bcp;
	zs->DEP = cpu->dep;
	zs->HLP = cpu->hlp;

	zs->PC = cpu->pc;
	zs->SP = cpu->sp;

	zs->I = cpu->i;
	zs->R = cpu->r;
	zs->Bus = cpu->bus;
	zs->IFF1 = cpu->iff1;
	zs->IFF2 = cpu->iff2;

	zs->IMode = cpu->imode;
	zs->SecondsElapsed = tc_elapsed(cpu->timer_c);

	zs->Frequency = calcs[gslot].timer_c.freq;
	zs->Halt = cpu->halt;

	return S_OK;
}

static HRESULT STDMETHODCALLTYPE
iw_setstate(WB_IWabbitemu *This, ULONG slot, Z80State_t *zs)
{
	BOOL running_backup = calcs[slot].running;

	calcs[slot].running = FALSE;
	CPU_t *cpu = &calcs[slot].cpu;

	cpu->af = zs->AF;
	cpu->bc = zs->BC;
	cpu->de = zs->DE;
	cpu->hl = zs->HL;

	cpu->afp = zs->AFP;
	cpu->bcp = zs->BCP;
	cpu->dep = zs->DEP;
	cpu->hlp = zs->HLP;

	cpu->ix = zs->IX;
	cpu->iy = zs->IY;

	cpu->pc = zs->PC;
	cpu->sp = zs->SP;

	cpu->i = zs->I;
	cpu->iff1 = zs->IFF1;
	cpu->iff2 = zs->IFF2;

	cpu->imode = zs->IMode;

	calcs[slot].timer_c.freq = zs->Frequency;
	cpu->halt = zs->Halt;

	calcs[slot].running = running_backup;
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE
iw_keypress(WB_IWabbitemu *This, ULONG slot, DWORD vk) {
	keypad_key_press(&calcs[slot].cpu, vk);
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE
iw_keyrelease(WB_IWabbitemu *This, ULONG slot, DWORD vk) {
	keypad_key_release(&calcs[slot].cpu, vk);
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE
iw_memread(WB_IWabbitemu *This, ULONG slot, WORD address, LPBYTE value) {
	*value = mem_read(calcs[slot].cpu.mem_c, address);
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE
iw_memwrite(WB_IWabbitemu *This, ULONG slot, WORD address, BYTE value) {
	mem_write(calcs[slot].cpu.mem_c, address, value);
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE
iw_drawscreen(WB_IWabbitemu *This, ULONG slot, BYTE *Image) {
	BYTE *image = LCD_image(calcs[slot].cpu.pio.lcd);
	memcpy(Image, image, 128*64);
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE
iw_setbreakpoint(WB_IWabbitemu *This, ULONG slot, HWND hwndCallback,  BOOL is_ram, BYTE page, WORD address) {
	if (is_ram > TRUE)
		is_ram = TRUE;
	
	if (is_ram == TRUE) {
		if (page >= calcs[slot].mem_c.ram_pages)
			return E_INVALIDARG;
	} else {
		if (page >= calcs[slot].mem_c.flash_pages)
			return E_INVALIDARG;
	}
	
	calcs[slot].ole_callback = hwndCallback;
	
	set_break(&calcs[slot].mem_c, is_ram, page, address);
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE
iw_clearbreakpoint(WB_IWabbitemu *This, ULONG slot, BOOL is_ram, BYTE page, WORD address) {
	if (is_ram > TRUE)
		is_ram = TRUE;
	
	if (is_ram == TRUE) {
		if (page >= calcs[slot].mem_c.ram_pages)
			return E_INVALIDARG;
	} else {
		if (page >= calcs[slot].mem_c.flash_pages)
			return E_INVALIDARG;
	}
	
	calcs[slot].mem_c.breaks[is_ram % 2][PAGE_SIZE * page + mc_base(address)] = 0;
	//calcs[slot].running = TRUE;
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE
iw_getapplist(WB_IWabbitemu *This, ULONG slot, APPENTRY AppList[96], DWORD *Count) {
	if (AppList == NULL || Count == NULL) {
		return E_INVALIDARG;
	}

	applist_t applist;
	state_build_applist(&calcs[slot].cpu, &applist);

	int i;
	for (i = 0; i < applist.count && i < 96; i++) {
		printf("App: %s\n", applist.apps[i].name);
		strcpy(AppList[i].name, applist.apps[i].name);
		AppList[i].page = applist.apps[i].page;
		AppList[i].page_count = applist.apps[i].page_count;
	}

	if (i < applist.count) {
		return E_FAIL;
	}

	*Count = applist.count;
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE
iw_getsymlist(WB_IWabbitemu *This, ULONG slot, SYMENTRY SymList[256], DWORD *Count) {
	if (SymList == NULL || Count == NULL) {
		return E_INVALIDARG;
	}

	symlist_t symlist;
	state_build_symlist_83P(&calcs[slot].cpu, &symlist);

	int i;
	for(i=0;(&symlist.symbols[i])<=(symlist.last) && i < *Count;i++) {
		symbol83P_t *sym = &symlist.symbols[i];

		SymList[i].TypeID = sym->type_ID;
		Symbol_Name_to_String(sym, SymList[i].name);
		SymList[i].page = sym->page;
		SymList[i].address = sym->address;
		printf("sym: %s\n", SymList[i].name);
	}

	if (&symlist.symbols[i] > (symlist.last)) {
		return E_FAIL;
	}

	*Count = i;
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE
iw_getmemstate(WB_IWabbitemu *This, ULONG slot, MEMSTATE *mem) {
	int i;
	for (i = 0; i < 4; i++) {
		bank_t *bank = &calcs[slot].mem_c.banks[i];
		mem[i].is_ram = bank->ram;
		mem[i].page = bank->page;
	}

	return S_OK;
}

static HRESULT STDMETHODCALLTYPE
iw_setmemstate(WB_IWabbitemu *This, ULONG slot, MEMSTATE *mem) {
	int i;
	for (i = 0; i < 4; i++) {
		bank_t *bank = &calcs[slot].mem_c.banks[i];
		bank->ram = mem[i].is_ram;
		bank->page = mem[i].page;
	}

	return S_OK;
}

static HRESULT STDMETHODCALLTYPE
iw_step(WB_IWabbitemu *This, ULONG slot) {
	CPU_step(&calcs[slot].cpu);
	if (calcs[slot].hwndDebug != NULL)
		SendMessage(calcs[slot].hwndDebug, WM_USER, DB_UPDATE, 0);
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE
iw_stepover(WB_IWabbitemu *This, ULONG slot) {
	CPU_stepover(&calcs[slot].cpu);
	if (calcs[slot].hwndDebug != NULL)
		SendMessage(calcs[slot].hwndDebug, WM_USER, DB_UPDATE, 0);
	return S_OK;
	}
static HRESULT STDMETHODCALLTYPE
iw_stepout(WB_IWabbitemu *This, ULONG slot) {
	CPU_stepout(&calcs[slot].cpu);
	if (calcs[slot].hwndDebug != NULL)
		SendMessage(calcs[slot].hwndDebug, WM_USER, DB_UPDATE, 0);
	return S_OK;
}

static HRESULT STDMETHODCALLTYPE
iw_run(WB_IWabbitemu *This, ULONG slot) {
	calcs[slot].running = TRUE;
	return S_OK;
}
