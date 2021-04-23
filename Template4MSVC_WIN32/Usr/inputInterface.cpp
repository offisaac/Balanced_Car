#include "inputInterface.h"

static HHOOK  hhkMouse = NULL; //��깳�Ӿ��;
static HINSTANCE g_hInstDll = NULL; //��dll��ʵ�����;
static HWND g_hWnd = NULL;//����dll�������ھ��
mouseHook_t mouse;
keyboardHook_t keyboard;
HWND m_hWnd; //ȫ�ִ���

BOOL WINAPI StartHookMouse(HWND hWnd);
VOID WINAPI StopHookMouse();

LRESULT CALLBACK WindowProc(_In_  HWND hwnd, _In_  UINT uMsg, _In_  WPARAM wParam, _In_  LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		break;
	case WM_PAINT:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_INPUT:
	{
		UINT dwSize = 0;
		GetRawInputData((HRAWINPUT)lParam, (UINT)RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));//ȡ����,��һ�ε��ú�������ȡ��Ҫ���ֽڴ�С		
		LPBYTE lpbBuffer = new BYTE[dwSize];//����ָ���Ļ�������С
		GetRawInputData((HRAWINPUT)lParam, (UINT)RID_INPUT, (LPVOID)lpbBuffer, (PUINT)&dwSize, (UINT)sizeof(RAWINPUTHEADER));//�ڶ��ε��û�ȡԭʼ�������ݣ�����lpbBuffer

		RAWINPUT* raw = (RAWINPUT*)lpbBuffer;
		if (raw->header.dwType == RIM_TYPEKEYBOARD)//������Խػ����м�����Ϣ,�������ֲ�ͬ�ļ���������Ϣ,����ͨ���豸����жϡ�
		{
			//��ȡ��������
			UINT bufferSize;//Ϊ�����豸��׼����������С
			GetRawInputDeviceInfo(raw->header.hDevice, RIDI_DEVICENAME, NULL, &bufferSize);
			WCHAR* RawDevName = new WCHAR[bufferSize];
			GetRawInputDeviceInfo(raw->header.hDevice, RIDI_DEVICENAME, RawDevName, &bufferSize);//���豸�����뻺����RawDevName
			//printf("dev=%ls\n", RawDevName);
			delete[] RawDevName;

			//��ȡ��������
			char keytext[10] = { 0 };
			BYTE state[256] = { 0 };
			ToAscii(raw->data.keyboard.VKey, raw->data.keyboard.MakeCode, state, (LPWORD)keytext, 0);//ͨ�����������õ�����

			BYTE KeyIndex = 16;
			switch (raw->data.keyboard.VKey)
			{
			case _W_KV:
				KeyIndex = 0;
				break;
			case _S_KV:
				KeyIndex = 1;
				break;
			case _A_KV:
				KeyIndex = 2;
				break;
			case _D_KV:
				KeyIndex = 3;
				break;
			case _SHIFT_KV:
				KeyIndex = 4;
				break;
			case _CTRL_KV:
				KeyIndex = 5;
				break;
			case _Q_KV:
				KeyIndex = 6;
				break;
			case _E_KV:
				KeyIndex = 7;
				break;
			case _R_KV:
				KeyIndex = 8;
				break;
			case _F_KV:
				KeyIndex = 9;
				break;
			case _G_KV:
				KeyIndex = 10;
				break;
			case _Z_KV:
				KeyIndex = 11;
				break;
			case _X_KV:
				KeyIndex = 12;
				break;
			case _C_KV:
				KeyIndex = 13;
				break;
			case _V_KV:
				KeyIndex = 14;
				break;
			case _B_KV:
				KeyIndex = 15;
				break;
			case _ESC_KV:
				KeyIndex = 18;
				break;
			case _O_KV:
				KeyIndex = 19;
				break;
			case _P_KV:
				KeyIndex = 20;
				break;
			case _F7_KV:
				KeyIndex = 21;
				break;
			}

			if (raw->data.keyboard.Message == WM_KEYDOWN)
			{
				if (KeyIndex < 32)
				{
					keyboard.setKeyboardMask(KeyIndex);
				}
			}
			else if (raw->data.keyboard.Message == WM_KEYUP)
			{
				keyboard.resetKeyboardMask(KeyIndex);
			}
		}
		delete[] lpbBuffer;
		return 0;
	}
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

void keyboardHook_t::RegKeyboardRawInput(HWND hWnd)
{
	RAWINPUTDEVICE rawInputDevice[1];
	rawInputDevice[0].usUsagePage = 0x01;//�豸��
	rawInputDevice[0].usUsage = 0x06;//�豸���ڵľ����豸
	rawInputDevice[0].dwFlags = RIDEV_INPUTSINK;//��ζ�ż�ʹ����ʧȥ����λ�ã���Ȼ��һֱ����������Ϣ
	rawInputDevice[0].hwndTarget = hWnd;
	if (RegisterRawInputDevices(rawInputDevice, 1, sizeof(rawInputDevice[0])) == FALSE)
	{
		printf_s(" - RegisterRawInputDevices failed\r\n");
	}
}

int keyboardHook_t::init()
{
	/* �������� */
	HINSTANCE hInst;

	hInst = GetModuleHandle(NULL);	//��ȡһ��Ӧ�ó����̬���ӿ��ģ����	
	WNDCLASSEX wcx;
	wcx.cbSize = sizeof(wcx);
	wcx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_NOCLOSE;//���ڵķ��	
	wcx.lpfnWndProc = WindowProc;     //�����Զ�����Ϣ������
	wcx.cbClsExtra = 0;                // no extra class memory  
	wcx.cbWndExtra = 0;                // no extra window memory  
	wcx.hInstance = hInst;         //��ǰӦ�ó����ʵ����� 
	wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION); //ͼ����		
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);  //�����
	wcx.hbrBackground = (HBRUSH)WHITE_BRUSH; //����ɫ
	wcx.lpszMenuName = NULL;  //�˵���	
	wcx.lpszClassName = _T("ITSMYOSDCLASS"); //�����������
	wcx.hIconSm = NULL;

	if (!RegisterClassEx(&wcx))
	{
		printf_s(" - Register classEx failed!\r\n");
		return -1;
	}
	else printf_s(" - Register classEx succeed!\r\n");

	//��������Ļ�ϵ���ʾλ��
	int OSDleft = GetSystemMetrics(SM_CXSCREEN) / 2 - 300;
	int OSDTop = GetSystemMetrics(SM_CYSCREEN) / 2;

	m_hWnd = CreateWindowEx(
		WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,//���ڵ���չ���
		wcx.lpszClassName,//����ע�������lpszClassName��Ҫ��ȫһ��  
		NULL,
		WS_VISIBLE | WS_POPUP,//���ڵķ��
		OSDleft,//��������ڸ�����X����  
		OSDTop,//��������ڸ�����Y���� 
		300,//���ڵĿ��  
		300,//���ڵĸ߶� 
		(HWND)NULL,//û�и����ڣ�Ϊ(HWND)NULL//GetDesktopWindow(),
		(HMENU)NULL,//û�в˵���ΪNULL  
		hInst,//��ǰӦ�ó����ʵ�����  
		(LPVOID)NULL);	//û�и������ݣ�ΪNULL  

	if (!m_hWnd)
	{
		printf_s(" - Create window failed!\r\n");
		return -1;
	}
	else printf_s(" - Create window succeed!\r\n");

	RegKeyboardRawInput(m_hWnd);

}

void keyboardHook_t::setKeyboardMask(INT index)
{
	if (index < 32 && index >= 0)
	{
		this->keyPressedMask |= (1 << index);
		this->keyTriggeredMask |= (1 << index);
	}
}

void keyboardHook_t::resetKeyboardMask(INT index)
{
	if (index < 32 && index >= 0)
	{
		this->keyPressedMask &= ~(1 << index);
	}
}

BYTE keyboardHook_t::isKeyPressed(BYTE key)
{
	if (this->keyPressedMask & (1 << key))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

BYTE keyboardHook_t::isKeyTriggered(BYTE key)
{
	if (this->keyTriggeredMask & (1 << key))
	{
		this->keyTriggeredMask &= ~(1 << key);
		return 1;
	}
	else
	{
		return 0;
	}
}

mouseHook_t::mouseHook_t()
{

}

mouseHook_t::~mouseHook_t()
{
	StopHookMouse();
}

void mouseHook_t::init()
{
	StartHookMouse(m_hWnd);
	ClipCursor(NULL);
}

void mouseHook_t::updateMousePos(CPoint pos)
{
	this->lastPos = this->mousePos;
	this->mousePos = pos;
	this->timeWithoutUpdate = 0;
}

void mouseHook_t::setMousePressed(BYTE lr)
{
	this->mousePressMask |= (1 << (lr - 16));
}

void mouseHook_t::resetMousePressed(BYTE lr)
{
	this->mousePressMask &= ~(1 << (lr - 16));
}

bool mouseHook_t::ifMousePressed(BYTE lr)
{
	if (this->mousePressMask & (1 << (lr - 16)))
		return true;
	else
		return false;
}

void mouseHook_t::setExitFlag(bool exflag)
{
	this->exitFlag = exflag;
}

/* ���������ٶ� */
void mouseHook_t::resolveVelocity(float timegap)
{
	this->velocity.vx = (this->mousePos.x - this->lastPos.x) / timegap;
	this->velocity.vy = (this->mousePos.y - this->lastPos.y) / timegap;
	if (mousePos.x < 800 || mousePos.x>1120 || mousePos.y < 400 || mousePos.y>680)
	{
		if (!exitFlag)
		{
			mousePos.x = 1920 / 2;
			mousePos.y = 1080 / 2;
			lastPos = mousePos;
			SetCursorPos(mousePos.x, mousePos.y);
			//printf_s("%d,%d\r\n", mousePos.x, mousePos.y);
		}
	}
	timeWithoutUpdate++;
	if (timeWithoutUpdate > 10)
	{
		velocity.vx = velocity.vy = 0;
	}
}

//�ͼ���깳�ӹ���
LRESULT CALLBACK LowLevelMouseProc(
	int nCode,      // hook code
	WPARAM wParam,  // message identifier
	LPARAM lParam   // mouse coordinates
)
{
	CPoint  _mousepoint;
	//�������Ϣʱ�����䷢��������
	if (g_hWnd != NULL && nCode == HC_ACTION)
	{
		GetCursorPos(&_mousepoint);
		mouse.updateMousePos(_mousepoint);
		switch (wParam)
		{
		case WM_LBUTTONDOWN:
			mouse.setMousePressed(_MOUSE_L);
			break;
		case WM_LBUTTONUP:
			mouse.resetMousePressed(_MOUSE_L);
			break;
		case WM_RBUTTONDOWN:
			mouse.setMousePressed(_MOUSE_R);
			break;
		case WM_RBUTTONUP:
			mouse.resetMousePressed(_MOUSE_R);
			break;
		default:
			break;
		}
	}
	return CallNextHookEx(hhkMouse, nCode, wParam, lParam);
}

//��װ�ͼ���깳�ӣ��Ӷ��ػ�ϵͳ���е������Ϣ
BOOL WINAPI StartHookMouse(HWND hWnd)
{
	g_hWnd = hWnd;
	hhkMouse = ::SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, g_hInstDll, 0);
	if (hhkMouse == NULL)
	{
		printf_s(" - Start mouse hook failed!\r\n");
		return FALSE;
	}
	else
	{
		printf_s(" - Start mouse hook successfully!\r\n");
		return TRUE;
	}
}

//ж�صͼ���깳��
VOID WINAPI StopHookMouse()
{
	if (hhkMouse != NULL)
	{
		::UnhookWindowsHookEx(hhkMouse);
	}
}
