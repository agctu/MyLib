#ifndef DXGI_SCREEN_CAPTURE_HPP
#define DXGI_SCREEN_CAPTURE_HPP
#include <dxgi1_2.h>
#include <d3d11.h>
#include <iostream>
#include <assert.h>
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3d11.lib")
using namespace std;
	typedef struct _FRAME_DATA
	{
		ID3D11Texture2D* Frame;
		DXGI_OUTDUPL_FRAME_INFO FrameInfo;
		_Field_size_bytes_((MoveCount * sizeof(DXGI_OUTDUPL_MOVE_RECT)) + (DirtyCount * sizeof(RECT))) BYTE* MetaData;
		UINT DirtyCount;
		UINT MoveCount;
	} FRAME_DATA;
namespace mine {
	class duplicator {
		IDXGIOutputDuplication *deskdupl;
		ID3D11Texture2D *acquireddeskimage;
		UINT metadatasize;
		UINT outputnumber;
		BYTE *metadatabuf;
		DXGI_OUTPUT_DESC outputdesc;
		ID3D11Texture2D *stagingtex;
		ID3D11Device *device;
		ID3D11DeviceContext *d3dcontext;
	public:
		duplicator()
		{
			deskdupl = nullptr;
			acquireddeskimage = nullptr;
			metadatasize = 0;
			outputnumber = 0;
			metadatabuf = nullptr;
			RtlZeroMemory(&outputdesc, sizeof(outputdesc));
			stagingtex = nullptr;
			device = nullptr;
			d3dcontext = nullptr;
		}
		bool initdevice()
		{
			HRESULT hr;
			IDXGIFactory1 *factory = nullptr;
			hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&factory));
			if (hr != S_OK)
				return false;
			//vector<IDXGIAdapter1*>adapters;
			IDXGIAdapter1 *adapter;
			hr= factory->EnumAdapters1(0, &adapter);
			if (hr != S_OK)
				return false;
			DXGI_ADAPTER_DESC desc;
			hr=adapter->GetDesc(&desc);
			if (hr != S_OK)
				return false;
			//cout << desc.Description << endl;
			IDXGIOutput *output = nullptr;
			hr=adapter->EnumOutputs(0, &output);
			if (hr != S_OK)
				return false;
			DXGI_OUTPUT_DESC odesc;
			hr = output->GetDesc(&odesc);
			if (hr != S_OK)
				return false;
			//cout << odesc.DeviceName << " "<<(odesc.AttachedToDesktop ?  'y': 'n') << endl;
			//ID3D11DeviceContext *context = nullptr;
			D3D_FEATURE_LEVEL d3dfl;
			hr = D3D11CreateDevice(NULL,
				D3D_DRIVER_TYPE_HARDWARE,
				nullptr, 0, nullptr,
				0,
				D3D11_SDK_VERSION,
				&device,
				&d3dfl,
				&d3dcontext);
			if (hr != S_OK)
				return false;


			return true;
		}
		bool initdupl()
		{
			HRESULT hr;
			outputnumber = 0;//wheather should be like this is not clear
			IDXGIDevice *dxgidev = nullptr;
			hr = device->QueryInterface(__uuidof(IDXGIDevice), (void**)(&dxgidev));
			if (hr != S_OK)
				return false;
			IDXGIAdapter *dxgiadapter = nullptr;
			hr = dxgidev->GetParent(__uuidof(IDXGIAdapter), (void**)(&dxgiadapter));
			dxgidev->Release();
			dxgidev = nullptr;
			if (hr != S_OK)
				return false;
			IDXGIOutput *dxgioutput = nullptr;
			hr = dxgiadapter->EnumOutputs(0, &dxgioutput);
			dxgiadapter->Release();
			dxgiadapter = nullptr;
			if (hr != S_OK)
				return false;
			dxgioutput->GetDesc(&outputdesc);

			IDXGIOutput1 *dxgioutput1 = nullptr;
			hr = dxgioutput->QueryInterface(__uuidof(dxgioutput1), (void**)&dxgioutput1);
			dxgioutput->Release();
			dxgioutput = nullptr;
			if (hr != S_OK)
				return false;
			hr = dxgioutput1->DuplicateOutput(device, &deskdupl);
			dxgioutput1->Release();
			dxgioutput1 = nullptr;
			if (hr != S_OK)
				return false;

			D3D11_TEXTURE2D_DESC texdesc;
			texdesc.Width = outputdesc.DesktopCoordinates.right;
			texdesc.Height = outputdesc.DesktopCoordinates.bottom;
			texdesc.MipLevels = 1;
			texdesc.ArraySize = 1;
			texdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			texdesc.SampleDesc.Count = 1;//multisampling
			texdesc.SampleDesc.Quality = 0;
			texdesc.Usage = D3D11_USAGE_STAGING;
			texdesc.BindFlags = 0;
			texdesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			texdesc.MiscFlags = 0;

			hr = device->CreateTexture2D(&texdesc, NULL, &stagingtex);
			if (hr != S_OK)
				return false;

			return true;
		}
		bool getframe(void* receiver,bool& timeout)
		{
			/*if (width > getwidth())
				return false;
			if (height > getheight())
				return false;*/
			FRAME_DATA data;
			HRESULT hr;
			IDXGIResource *deskres = nullptr;
			DXGI_OUTDUPL_FRAME_INFO info;

			hr = deskdupl->AcquireNextFrame(500, &info, &deskres);
			if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
				timeout = true;
				return true;
			}
			
			timeout = false;
			//destroy the old frame
			if (acquireddeskimage) {
				acquireddeskimage->Release();
				acquireddeskimage = nullptr;
			}

			hr = deskres->QueryInterface(__uuidof(ID3D11Texture2D), (void**)(&acquireddeskimage));
			deskres->Release();
			deskres = nullptr;
			if (hr != S_OK)
				return false;
			//if (info.TotalMetadataBufferSize) {
			//	if (info.TotalMetadataBufferSize > metadatasize) {
			//		if (metadatabuf) {
			//			delete[] metadatabuf;
			//			metadatabuf = nullptr;
			//		}
			//		metadatabuf = new (std::nothrow) BYTE[info.TotalMetadataBufferSize];
			//		if (!metadatabuf) {
			//			metadatasize = 0;
			//			data.MoveCount = 0;
			//			data.DirtyCount = 0;
			//			return false;
			//		}
			//		metadatasize = info.TotalMetadataBufferSize;
			//	}

			//	UINT bufsize = info.TotalMetadataBufferSize;

			//	hr = deskdupl->GetFrameMoveRects(bufsize, (DXGI_OUTDUPL_MOVE_RECT*)(metadatabuf), &bufsize);

			//	if (hr != S_OK) {
			//		data.MoveCount = 0;
			//		data.DirtyCount = 0;
			//		return false;
			//	}
			//	data.MoveCount = bufsize / sizeof(DXGI_OUTDUPL_MOVE_RECT);
			//	BYTE *dirtyrects = metadatabuf + bufsize;
			//	bufsize = info.TotalMetadataBufferSize - bufsize;

			//	hr = deskdupl->GetFrameDirtyRects(bufsize, (RECT*)dirtyrects,&bufsize);
			//	if (hr != S_OK) {
			//		data.MoveCount = 0;
			//		data.DirtyCount = 0;
			//		return false;
			//	}
			//
			//	data.DirtyCount = bufsize / sizeof(RECT);

			//	/*{
			//		printf("data start:\n");
			//		RECT *r = (RECT*)dirtyrects;
			//		for (int i = 0; i < data.DirtyCount; i++) {
			//			RECT rect = *(r + i);
			//			printf("top:%d,bottom:%d,left:%d,right:%d\n", rect.top, rect.bottom, rect.left, rect.right);
			//		}
			//	}*/

			//	data.MetaData = metadatabuf;

			//}
			//data.Frame = acquireddeskimage;
			//data.FrameInfo = info;


			//below get the real pixels from all of all that
			DXGI_MAPPED_RECT mappedrect;
			hr = deskdupl->MapDesktopSurface(&mappedrect);

			if (hr != DXGI_ERROR_UNSUPPORTED)
				return false;
			
			
			

			d3dcontext->CopyResource(stagingtex, acquireddeskimage);
			D3D11_MAPPED_SUBRESOURCE map;
			hr = d3dcontext->Map(stagingtex, 0, D3D11_MAP_READ, 0, &map);
			if (hr != S_OK)
				return false;
			BYTE *rdata = (BYTE*)map.pData;
			
			
			/*for (int i = 0; i < 5; i++) {
				cout << (int)rdata[i * 4]<<"  " << (int)rdata[i * 4 + 1]<<"  " << (int)rdata[i * 4 + 2]<<"  " <<(int) rdata[i * 4 + 3] << endl;
			}*/
			BYTE* out = (BYTE*)receiver;
			for(int i=0;i<getwidth()*getheight();i++)
				for (int j = 0; j < 3; j++) {
					out[i * 3 + j] = rdata[i * 4 + j];
				}
			
			/*float xscale = (float)getwidth() / width;
			float yscale = (float)getheight() / height;
			for(int x=0;x<width;x++)
				for (int y = 0; y < height; y++) {
					for (int k = 0; k < 3; k++) {
						out[3 * (x + width * y) + k] = rdata[4 * (int(xscale*x) + int(yscale * y)*getwidth()) + k];
					}
				}*/

			//d3dcontext->Unmap(stagingtex, 0);
			done();
			return true;
		}
		int getwidth()
		{
			return outputdesc.DesktopCoordinates.right;
		}
		int getheight()
		{
			return outputdesc.DesktopCoordinates.bottom;
		}
		void done()
		{
			deskdupl->ReleaseFrame();

			if (acquireddeskimage) {
				acquireddeskimage->Release();
				acquireddeskimage = nullptr;
			}
		}
		~duplicator()
		{
			if (deskdupl) {
				deskdupl->Release();
				deskdupl = nullptr;
			}

			if (acquireddeskimage) {
				acquireddeskimage->Release();
				acquireddeskimage = nullptr;
			}

			if (metadatabuf) {
				delete[] metadatabuf;
				metadatabuf = nullptr;
			}

			if (stagingtex) {
				stagingtex->Release();
				stagingtex = nullptr;
			}
			if (device) {
				device->Release();
				device = nullptr;
			}

			if (d3dcontext) {
				d3dcontext->Release();
				d3dcontext = nullptr;
			}
		}
	};
}
#endif