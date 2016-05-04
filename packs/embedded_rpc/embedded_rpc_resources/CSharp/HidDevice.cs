using System;
using System.Collections.Generic;
using System.Collections;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;
using System.IO;
using Microsoft.Win32.SafeHandles;



namespace PInS.HID
{
    class DataTypes
    {
        [StructLayout(LayoutKind.Sequential)]
        public unsafe struct COMMTIMEOUTS
        {
            public UInt32 ReadIntervalTimeout;
            public UInt32 ReadTotalTimeoutMultiplier;
            public UInt32 ReadTotalTimeoutConstant;
            public UInt32 WriteTotalTimeoutMultiplier;
            public UInt32 WriteTotalTimeoutConstant;
        } 

        [StructLayout(LayoutKind.Sequential)]
        public unsafe struct GUID
        {
            public int Data1;
            public System.UInt16 Data2;
            public System.UInt16 Data3;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
            public byte[] data4;
        }

		// Device interface data
		[StructLayout(LayoutKind.Sequential)]
		public unsafe struct SP_DEVICE_INTERFACE_DATA 
		{
			public int cbSize;
			public GUID InterfaceClassGuid;
			public int Flags;
			public int Reserved;
		}
		
		// Device interface detail data
		[StructLayout(LayoutKind.Sequential, CharSet= CharSet.Ansi)]
		public unsafe struct PSP_DEVICE_INTERFACE_DETAIL_DATA
		{
			public int  cbSize;
			[MarshalAs(UnmanagedType.ByValTStr, SizeConst= 256)]
			public string DevicePath;
		}

		// HIDD_ATTRIBUTES
		[StructLayout(LayoutKind.Sequential)]
		public unsafe struct HIDD_ATTRIBUTES 
		{
			public int   Size; // = sizeof (struct _HIDD_ATTRIBUTES) = 10

			//
			// Vendor ids of this hid device
			//
			public System.UInt16	VendorID;
			public System.UInt16	ProductID;
			public System.UInt16	VersionNumber;

			//
			// Additional fields will be added to the end of this structure.
			//
		} 

		// HIDP_CAPS
		[StructLayout(LayoutKind.Sequential)]
		public unsafe struct HIDP_CAPS 
		{
			public System.UInt16  Usage;					// USHORT
			public System.UInt16   UsagePage;				// USHORT
			public System.UInt16   InputReportByteLength;
			public System.UInt16   OutputReportByteLength;
			public System.UInt16   FeatureReportByteLength;
			[MarshalAs(UnmanagedType.ByValArray, SizeConst=17)]
			public System.UInt16[] Reserved;				// USHORT  Reserved[17];			
			public System.UInt16  NumberLinkCollectionNodes;
			public System.UInt16  NumberInputButtonCaps;
			public System.UInt16  NumberInputValueCaps;
			public System.UInt16  NumberInputDataIndices;
			public System.UInt16  NumberOutputButtonCaps;
			public System.UInt16  NumberOutputValueCaps;
			public System.UInt16  NumberOutputDataIndices;
			public System.UInt16  NumberFeatureButtonCaps;
			public System.UInt16  NumberFeatureValueCaps;
			public System.UInt16  NumberFeatureDataIndices;
		}
		
		//HIDP_REPORT_TYPE 
		public enum HIDP_REPORT_TYPE 
		{
			HidP_Input,		// 0 input
			HidP_Output,	// 1 output
			HidP_Feature	// 2 feature
		}

		// Structures in the union belonging to HIDP_VALUE_CAPS (see below)

		// Range
		[StructLayout(LayoutKind.Sequential)]
		public unsafe struct Range 
		{
			public System.UInt16 UsageMin;			// USAGE	UsageMin; // USAGE  Usage; 
			public System.UInt16 UsageMax; 			// USAGE	UsageMax; // USAGE	Reserved1;
			public System.UInt16 StringMin;			// USHORT  StringMin; // StringIndex; 
			public System.UInt16 StringMax;			// USHORT	StringMax;// Reserved2;
			public System.UInt16 DesignatorMin;		// USHORT  DesignatorMin; // DesignatorIndex; 
			public System.UInt16 DesignatorMax;		// USHORT	DesignatorMax; //Reserved3; 
			public System.UInt16 DataIndexMin;		// USHORT  DataIndexMin;  // DataIndex; 
			public System.UInt16 DataIndexMax;		// USHORT	DataIndexMax; // Reserved4;
		}

		// Range
		[StructLayout(LayoutKind.Sequential)]
		public unsafe struct NotRange 
		{
			public System.UInt16 Usage; 
			public System.UInt16 Reserved1;
			public System.UInt16 StringIndex; 
			public System.UInt16 Reserved2;
			public System.UInt16 DesignatorIndex; 
			public System.UInt16 Reserved3; 
			public System.UInt16 DataIndex; 
			public System.UInt16 Reserved4;
		}
        
        //HIDP_VALUE_CAPS
		[StructLayout(LayoutKind.Explicit, CharSet= CharSet.Ansi)]
		public unsafe struct HIDP_VALUE_CAPS 
		{
			//
			[FieldOffset(0)] public System.UInt16  UsagePage;					// USHORT
			[FieldOffset(2)] public System.Byte ReportID;						// UCHAR  ReportID;
			[MarshalAs(UnmanagedType.I1)]
			[FieldOffset(3)] public System.Boolean IsAlias;						// BOOLEAN  IsAlias;
			[FieldOffset(4)] public System.UInt16 BitField;						// USHORT  BitField;
			[FieldOffset(6)] public System.UInt16 LinkCollection;				//USHORT  LinkCollection;
			[FieldOffset(8)] public System.UInt16 LinkUsage;					// USAGE  LinkUsage;
			[FieldOffset(10)] public System.UInt16 LinkUsagePage;				// USAGE  LinkUsagePage;
			[MarshalAs(UnmanagedType.I1)]
			[FieldOffset(12)] public System.Boolean IsRange;					// BOOLEAN  IsRange;
			[MarshalAs(UnmanagedType.I1)]
			[FieldOffset(13)] public System.Boolean IsStringRange;				// BOOLEAN  IsStringRange;
			[MarshalAs(UnmanagedType.I1)]
			[FieldOffset(14)] public System.Boolean IsDesignatorRange;			// BOOLEAN  IsDesignatorRange;
			[MarshalAs(UnmanagedType.I1)]
			[FieldOffset(15)] public System.Boolean IsAbsolute;					// BOOLEAN  IsAbsolute;
			[MarshalAs(UnmanagedType.I1)]
			[FieldOffset(16)] public System.Boolean HasNull;					// BOOLEAN  HasNull;
			[FieldOffset(17)] public System.Char Reserved;						// UCHAR  Reserved;
			[FieldOffset(18)] public System.UInt16 BitSize;						// USHORT  BitSize;
			[FieldOffset(20)] public System.UInt16 ReportCount;					// USHORT  ReportCount;
			[FieldOffset(22)] public System.UInt16  Reserved2a;					// USHORT  Reserved2[5];		
			[FieldOffset(24)] public System.UInt16  Reserved2b;					// USHORT  Reserved2[5];
			[FieldOffset(26)] public System.UInt16  Reserved2c;					// USHORT  Reserved2[5];
			[FieldOffset(28)] public System.UInt16  Reserved2d;					// USHORT  Reserved2[5];
			[FieldOffset(30)] public System.UInt16  Reserved2e;					// USHORT  Reserved2[5];
			[FieldOffset(32)] public System.UInt16 UnitsExp;					// ULONG  UnitsExp;
			[FieldOffset(34)] public System.UInt16 Units;						// ULONG  Units;
			[FieldOffset(36)] public System.Int16 LogicalMin;					// LONG  LogicalMin;   ;
			[FieldOffset(38)] public System.Int16 LogicalMax;					// LONG  LogicalMax
			[FieldOffset(40)] public System.Int16 PhysicalMin;					// LONG  PhysicalMin, 
			[FieldOffset(42)] public System.Int16 PhysicalMax;					// LONG  PhysicalMax;
			// The Structs in the Union			
			[FieldOffset(44)] public Range Range;
			[FieldOffset(44)] public Range NotRange;				
		} 

    }

    class Kernel
    {
        public const uint GENERIC_READ = 0x80000000;
        public const uint GENERIC_WRITE = 0x40000000;
        public const uint FILE_SHARE_READ = 0x00000001;
        public const uint FILE_SHARE_WRITE = 0x00000002;
        public const int OPEN_EXISTING = 3;
        public const int EV_RXFLAG = 0x0002;    // received certain character

        public const int HIDP_STATUS_SUCCESS = (0x0 << 28) | (0x11 << 16) | 0;
        public const int HIDP_STATUS_NULL = (0x8 << 28) | (0x11 << 16) | 1;
        public const int HIDP_STATUS_INVALID_PREPARSED_DATA = (0xC << 28) | (0x11 << 16) | 1;
        public const int HIDP_STATUS_INVALID_REPORT_TYPE = (0xC << 28) | (0x11 << 16) | 2;
        public const int HIDP_STATUS_INVALID_REPORT_LENGTH = (0xC << 28) | (0x11 << 16) | 3;
        public const int HIDP_STATUS_USAGE_NOT_FOUND = (0xC << 28) | (0x11 << 16) | 4;
        public const int HIDP_STATUS_VALUE_OUT_OF_RANGE = (0xC << 28) | (0x11 << 16) | 5;
        public const int HIDP_STATUS_BAD_LOG_PHY_VALUES = (0xC << 28) | (0x11 << 16) | 6;
        public const int HIDP_STATUS_BUFFER_TOO_SMALL = (0xC << 28) | (0x11 << 16) | 7;
        public const int HIDP_STATUS_INTERNAL_ERROR = (0xC << 28) | (0x11 << 16) | 8;
        public const int HIDP_STATUS_I8042_TRANS_UNKNOWN = (0xC << 28) | (0x11 << 16) | 9;
        public const int HIDP_STATUS_INCOMPATIBLE_REPORT_ID = (0xC << 28) | (0x11 << 16) | 0xA;
        public const int HIDP_STATUS_NOT_VALUE_ARRAY = (0xC << 28) | (0x11 << 16) | 0xB;
        public const int HIDP_STATUS_IS_VALUE_ARRAY = (0xC << 28) | (0x11 << 16) | 0xC;
        public const int HIDP_STATUS_DATA_INDEX_NOT_FOUND = (0xC << 28) | (0x11 << 16) | 0xD;
        public const int HIDP_STATUS_DATA_INDEX_OUT_OF_RANGE = (0xC << 28) | (0x11 << 16) | 0xE;
        public const int HIDP_STATUS_BUTTON_NOT_PRESSED = (0xC << 28) | (0x11 << 16) | 0xF;
        public const int HIDP_STATUS_REPORT_DOES_NOT_EXIST = (0xC << 28) | (0x11 << 16) | 0x10;
        public const int HIDP_STATUS_NOT_IMPLEMENTED = (0xC << 28) | (0x11 << 16) | 0x20;

        // Get Create File
        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern SafeFileHandle CreateFile(
            string lpFileName,							// file name
            uint dwDesiredAccess,						// access mode
            uint dwShareMode,							// share mode
            uint lpSecurityAttributes,					// SD
            uint dwCreationDisposition,					// how to create
            uint dwFlagsAndAttributes,					// file attributes
            uint hTemplateFile							// handle to template file
            );

        [DllImport("kernel32.dll")]
        public static extern int CloseHandle(SafeHandle hObject);

        [DllImport("kernel32.dll")]
        public unsafe static extern bool WriteFile(
            SafeHandle hFile, 
            byte[] lpBuffer, 
            int nNumberOfBytesToWrite, 
            ref int lpNumberOfBytesWritten, 
            int* lpOverlapped);

        [DllImport("kernel32.dll", SetLastError = true)]
        public unsafe static extern bool ReadFile(
            SafeHandle hFile,						// handle to file
            byte[] lpBuffer,				// data buffer
            int nNumberOfBytesToRead,		// number of bytes to read
            ref int lpNumberOfBytesRead,	// number of bytes read
            int* ptr		// overlapped buffer
            );

        [DllImport("kernel32.dll", SetLastError = true)]
        public unsafe static extern bool CancelIoEx(SafeHandle hFile, IntPtr lpOverlapped);

    }

    class HidD
    {

        [DllImport("hid.dll", SetLastError=true, EntryPoint="HidD_GetHidGuid")]
        public static extern unsafe void GetHidGuid(
            ref DataTypes.GUID lpHidGuid);

        [DllImport("hid.dll", SetLastError = true, EntryPoint = "HidD_GetPreparsedData")]
        public unsafe static extern bool GetPreparsedData(
            SafeHandle hObject,								// IN HANDLE  HidDeviceObject,
            ref int pPHIDP_PREPARSED_DATA);				// OUT PHIDP_PREPARSED_DATA  *PreparsedData

        [DllImport("hid.dll", SetLastError = true, EntryPoint = "HidP_GetCaps")]
        public unsafe static extern int GetCaps(
            int pPHIDP_PREPARSED_DATA,					// IN PHIDP_PREPARSED_DATA  PreparsedData,
            ref DataTypes.HIDP_CAPS myPHIDP_CAPS);				// OUT PHIDP_CAPS  Capabilities

        [DllImport("hid.dll", SetLastError = true, EntryPoint = "HidD_FreePreparsedData")]
        public static extern unsafe bool FreePreparsedData(
            int pPHIDP_PREPARSED_DATA			// IN PHIDP_PREPARSED_DATA  PreparsedData
            );

        [DllImport("hid.dll", CharSet = CharSet.Unicode, SetLastError = true, EntryPoint = "HidD_GetProductString")]
        public static extern bool GetProductString(
            SafeHandle hObject,								// IN HANDLE  HidDeviceObject,
            StringBuilder lpBuffer,
            uint BufferLength
            );

        [DllImport("hid.dll", CharSet = CharSet.Unicode, SetLastError = true, EntryPoint="HidD_GetSerialNumberString")]
        public static extern bool GetSerialNumberString(
            SafeHandle HidDeviceObject,
            StringBuilder Buffer,
            uint BufferLength
            );

        [DllImport("hid.dll", SetLastError = true, EntryPoint = "HidD_GetAttributes")]
        public static extern bool GetAttributes(
            SafeHandle hObject,								// IN HANDLE  HidDeviceObject,
            ref DataTypes.HIDD_ATTRIBUTES Attributes);			// OUT PHIDD_ATTRIBUTES  Attributes

        [DllImport("hid.dll", SetLastError = true, EntryPoint = "HidD_SetOutputReport" )]
        public static extern bool SetOutputReport(SafeHandle HidDeviceObject, ref byte lpReportBuffer, int ReportBufferLength);

        [DllImport("hid.dll", SetLastError = true, EntryPoint = "HidD_GetInputReport" )]
        public static extern bool GetInputReport(SafeHandle HidDeviceObject, byte[] ReportBuffer, int ReportBufferLength);
    };

    class SetupDi
    {
		public const int  DIGCF_PRESENT				= 0x00000002;
		public const int  DIGCF_DEVICEINTERFACE		= 0x00000010;
		public const int  DIGCF_INTERFACEDEVICE		= 0x00000010;

		[DllImport("setupapi.dll", SetLastError=true, EntryPoint="SetupDiGetClassDevs")]
        public static extern unsafe IntPtr GetClassDevs(
			ref DataTypes.GUID  lpHidGuid,
			int*  Enumerator,
			int*  hwndParent,
			int  Flags);

		[DllImport("setupapi.dll", SetLastError=true, EntryPoint="SetupDiEnumDeviceInterfaces")]
		public static extern  unsafe int EnumDeviceInterfaces(
            IntPtr hDeviceInfoSet,
			int  DeviceInfoData,
			ref  DataTypes.GUID  lpHidGuid,
			int  MemberIndex,
			ref  DataTypes.SP_DEVICE_INTERFACE_DATA lpDeviceInterfaceData);

		[DllImport("setupapi.dll", SetLastError=true, EntryPoint="SetupDiGetDeviceInterfaceDetail")]
		public static extern  unsafe int GetDeviceInterfaceDetail(
            IntPtr hDeviceInfoSet,
			ref DataTypes.SP_DEVICE_INTERFACE_DATA lpDeviceInterfaceData,
			ref DataTypes.PSP_DEVICE_INTERFACE_DETAIL_DATA lpDeviceInterfaceDetailData,
			int detailSize,
			int* requiredSize,
			int* bPtr);

        [DllImport("setupapi.dll", SetLastError = true, EntryPoint = "SetupDiGetDeviceInterfaceDetail")]
        public static extern unsafe int GetDeviceInterfaceDetail(
            IntPtr hDeviceInfoSet,
            ref DataTypes.SP_DEVICE_INTERFACE_DATA lpDeviceInterfaceData,
            int* lpDeviceInterfaceDetailData,
            int detailSize,
            ref int requiredSize,
            int* bPtr);
    }


    public class HidDeviceEnumerator : IEnumerator<HidDevice>
    {

        public HidDeviceEnumerator() : this(0,0) { }
        public HidDeviceEnumerator(uint vendorID) : this (vendorID, 0) { }
        public HidDeviceEnumerator(uint vendorID, ushort productID)
        {
            if (vendorID!=0) mPIDVIDSearchString += "vid_" + vendorID.ToString("x4");
            mPIDVIDSearchString += "&";
            if (productID!=0) mPIDVIDSearchString += "pid_" + productID.ToString("x4");

            unsafe
            {
                HidD.GetHidGuid(ref mHidGUID);	// 
                mhDevInfo = SetupDi.GetClassDevs(
                    ref mHidGUID,
                    null,
                    null,
                    SetupDi.DIGCF_INTERFACEDEVICE | SetupDi.DIGCF_PRESENT);
            }

            Reset();
        }

        private string mPIDVIDSearchString = "";
        //--
        private DataTypes.GUID mHidGUID = new DataTypes.GUID();
        private IntPtr mhDevInfo;
        //-- iterator admin
        private int mNextDeviceIndex = 0;
        private HidDevice mCurrent = null;


        public bool MoveNext()
        {
            DataTypes.SP_DEVICE_INTERFACE_DATA  lDID = new DataTypes.SP_DEVICE_INTERFACE_DATA();
            lDID.cbSize = Marshal.SizeOf(lDID);

            mCurrent = null;
            bool hidDevFound;
            do
            {
                hidDevFound = (SetupDi.EnumDeviceInterfaces( mhDevInfo, 0, ref mHidGUID, mNextDeviceIndex, ref lDID)!=0);
                if (hidDevFound)
                {
                    DataTypes.PSP_DEVICE_INTERFACE_DETAIL_DATA lDIDD = new DataTypes.PSP_DEVICE_INTERFACE_DETAIL_DATA();
                    lDIDD.cbSize = 5;// Marshal.SizeOf(lDIDD);

                    unsafe {
			            int result = SetupDi.GetDeviceInterfaceDetail( mhDevInfo, ref lDID, ref lDIDD, Marshal.SizeOf(lDIDD), null, null);
                    }

                    if (lDIDD.DevicePath.IndexOf(mPIDVIDSearchString) > 0)
                    {
                        mCurrent = new HidDevice(lDIDD.DevicePath);
                    }

                    mNextDeviceIndex++;
                }
            }
            while (mCurrent == null && hidDevFound);

            return (mCurrent != null);
        }


        public HidDevice Current
        {
            get { return mCurrent; }
        }

        public void Reset() 
        {
            mNextDeviceIndex = 0;
            mCurrent = null;
        }

        void IDisposable.Dispose() 
        {
        }

        object IEnumerator.Current
        {
            get { return Current; }
        }
    }

    public class HidDevice
    {
        public string DevicePath { get; private set; }
        public string ProductName { get; private set; }
        public string SerialNumber { get; private set; }

        public ushort VendorID { get { return mHidAttributes.VendorID; } }
        public ushort ProductID { get { return mHidAttributes.ProductID; } }
        public ushort VersionNumber { get { return mHidAttributes.VersionNumber; } }
        public int OutReportLength { get { return mHidCaps.OutputReportByteLength-1; } }    // excl. report ID
        public int InReportLength { get { return mHidCaps.InputReportByteLength-1; } }      // excl. report ID

        private SafeFileHandle mDevHandle = new SafeFileHandle(new IntPtr(-1), true);
        private DataTypes.HIDP_CAPS mHidCaps;
        private DataTypes.HIDD_ATTRIBUTES mHidAttributes;

        public override string ToString()
        {
            return ProductName;
        }

        public HidDevice(string devicePath)
        {
            DevicePath = devicePath;
            mDevHandle.SetHandleAsInvalid();
        }

        public bool Connect()
        {
            if (!mDevHandle.IsClosed) return true;  // already connected

			mDevHandle = Kernel.CreateFile(
				DevicePath,
				Kernel.GENERIC_READ | Kernel.GENERIC_WRITE,
				Kernel.FILE_SHARE_READ | Kernel.FILE_SHARE_WRITE,
				0,
				Kernel.OPEN_EXISTING,
				0, 
				0);
            
            if (!mDevHandle.IsClosed)
            {
                // Get device capabilities
                int myPtrToPreparsedData = -1;
                bool ok = HidD.GetPreparsedData(mDevHandle, ref myPtrToPreparsedData);
                if (ok)
                {
                    mHidCaps = new DataTypes.HIDP_CAPS();
                    int result = HidD.GetCaps(myPtrToPreparsedData, ref mHidCaps);
                    HidD.FreePreparsedData(myPtrToPreparsedData);
                }

                unsafe
                {
                    mHidAttributes = new DataTypes.HIDD_ATTRIBUTES();
                    mHidAttributes.Size = sizeof(DataTypes.HIDD_ATTRIBUTES);
                    HidD.GetAttributes(mDevHandle, ref mHidAttributes);
                }

                StringBuilder name = new StringBuilder(100);
                HidD.GetProductString(mDevHandle, name, 100);
                ProductName = name.ToString();

                StringBuilder serial = new StringBuilder(100);
                HidD.GetSerialNumberString(mDevHandle, serial, 100);
                SerialNumber = serial.ToString();

                if (!ok)
                {
                    Disconnect();
                }
            }

            return (!mDevHandle.IsClosed);
        }

        public void Disconnect()
        {
            lock (this)
            {
                Kernel.CancelIoEx(mDevHandle, IntPtr.Zero);
                mDevHandle.Close();
            }
        }

        public bool SendOutputReport(byte[] reportData)
        {
            bool b = false;
            if ((!mDevHandle.IsClosed) && (reportData.Length <= OutReportLength))
            {
                lock (this) { } // prevent race with Disconnect()
                unsafe
                {
                    byte[] report = new byte[OutReportLength + 1];
                    reportData.CopyTo(report, 1);
                    report[0] = 0;
                    int nrWritten = 0;
                    b = Kernel.WriteFile(mDevHandle, report, report.Length, ref nrWritten, null);
                }
            }
            return b;
        }

        // Blocking!!
        public bool GetInputReport(out byte[] reportData)
        {
            bool b = false;
            reportData = null;
            if (!mDevHandle.IsClosed)
            {
                unsafe
                {
                    int BytesRead = 0;
                    byte[] BufBytes = new byte[InReportLength + 1];

                    lock (this) { } // prevent race with Disconnect()
                    if (Kernel.ReadFile(mDevHandle, BufBytes, BufBytes.Length, ref BytesRead, null))
                    {
                        b = true;
                        reportData = new byte[BytesRead - 1];
                        Array.ConstrainedCopy(BufBytes, 1, reportData, 0, BytesRead - 1);
                    }
                    Debug.Print(BytesRead.ToString() + " from device\n");
                }
            }
            return b;
        }

        public void AbortAllIO()
        {
            Kernel.CancelIoEx(mDevHandle, IntPtr.Zero);
        }

    }
}


