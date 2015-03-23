Imports System.Collections.ObjectModel
Imports WabbitemuLib
Imports SPASM
Imports System.Runtime.InteropServices
Imports System.Threading
Imports System.ComponentModel
Imports System.IO

Public Class GameModel
    Inherits DependencyObject
    Implements INotifyPropertyChanged
    'Implements IDisposable

    Private _Calc As Wabbitemu
    Private _Asm As IZ80Assembler
    Private _IsInitialized As Boolean = False
    Private _IsLoading As Boolean = False

    Private _RunningLock As New Object
    Private _Running As Boolean = True

    Public Const P_PUSHING As Byte = 1 << 2
    Public Const P_SHIELD As Byte = 1 << 4

    Public Shared ReadOnly ScreenXProperty As DependencyProperty =
        DependencyProperty.Register("ScreenX", GetType(Byte), GetType(GameModel))
    Public Shared ReadOnly ScreenYProperty As DependencyProperty =
        DependencyProperty.Register("ScreenY", GetType(Byte), GetType(GameModel))

    Public Shared ReadOnly SimulationCursorProperty As DependencyProperty =
        DependencyProperty.Register("SimulationCursor", GetType(Cursor), GetType(GameModel))

    Public ReadOnly Property IsInitialized As Boolean
        Get
            Return _IsInitialized
        End Get
    End Property

    Public ReadOnly Property IsLoading As Boolean
        Get
            Return _IsLoading
        End Get
    End Property


    Public Property EnemyScreen As Collection(Of Rect)
        Get
            Return GetValue(EnemyScreenProperty)
        End Get

        Set(ByVal value As Collection(Of Rect))
            SetValue(EnemyScreenProperty, value)
        End Set
    End Property

    Public Property Screen As Collection(Of Rect)
        Get
            Return GetValue(ScreenProperty)
        End Get

        Set(ByVal value As Collection(Of Rect))
            SetValue(ScreenProperty, value)
        End Set
    End Property

    Public ReadOnly Property ScreenBounds As Rect
        Get
            Return New Rect(ScreenX, ScreenY, 96, 64)
        End Get
    End Property


    Public Shared ReadOnly EnemyScreenProperty As DependencyProperty = _
                           DependencyProperty.Register("EnemyScreen", _
                           GetType(Collection(Of Rect)), GetType(GameModel), _
                           New PropertyMetadata(New Collection(Of Rect)))

    Public Shared ReadOnly ScreenProperty As DependencyProperty = _
                           DependencyProperty.Register("Screen", _
                           GetType(Collection(Of Rect)), GetType(GameModel), _
                           New PropertyMetadata(New Collection(Of Rect)))


    Public Shared ReadOnly CurrentMapProperty As DependencyProperty =
        DependencyProperty.Register("CurrentMap", GetType(Byte), GetType(GameModel))

    Public Shared ReadOnly DrawEntriesProperty As DependencyProperty =
        DependencyProperty.Register("DrawEntries", GetType(ObservableCollection(Of ZDrawEntry)), GetType(GameModel))

    Public Shared ReadOnly AnimatedTilesProperty As DependencyProperty =
        DependencyProperty.Register("AnimatedTiles", GetType(ObservableCollection(Of ZAnim)), GetType(GameModel))

    Public Shared ReadOnly MapProperty As DependencyProperty =
        DependencyProperty.Register("Map", GetType(MapData), GetType(GameModel))


    Public Property AnimatedTiles As ObservableCollection(Of ZAnim)
        Get
            Return GetValue(AnimatedTilesProperty)
        End Get
        Set(value As ObservableCollection(Of ZAnim))
            SetValue(AnimatedTilesProperty, value)
        End Set
    End Property

    Public ScreenX As Byte
    Public ScreenY As Byte

    Private _EnemyScreen As Byte()

    Public GameFlags As Byte

    ' Public PlayerX As Byte
    'Public PlayerY As Byte

    Public Property DrawEntries
        Get
            Return GetValue(DrawEntriesProperty)
        End Get
        Set(value)
            SetValue(DrawEntriesProperty, value)
        End Set
    End Property

    Public Property Map As MapData
        Get
            Return GetValue(MapProperty)
        End Get
        Set(value As MapData)
            SetValue(MapProperty, value)
        End Set
    End Property

    Private ZeldaApp As ITIApplication = Nothing
    Private ZeldaAppPage As Byte
    Private ImageMap As New Dictionary(Of UShort, Integer)

    Private DrawQueueAddr As UShort
    Private DrawEntrySize As Byte
    Private ScreenPosAddr As UShort
    Private DrawEntryCountAddr As UShort
    Private MapDataAddr As UShort
    Private FlagsAddr As UShort
    Private CurrentMapAddr As UShort
    Private ScreenPosEAddr As UShort
    Private TileTableAddr As UShort
    Private LastIndexAddr As UShort
    Private GameLoopAddr As UShort
    Private ResetGameAddr As UShort
    Private Memory As IMemoryContext

    Private AnimateArrayAddr As UShort
    Private AnimateAmount As Byte

    Private FrameProcessThread As New Thread(AddressOf FrameProcess)
    Private ProcessEvent As New AutoResetEvent(False)

    Private LoadMapCompleteCalcAddr As ICalcAddress
    Private PrepareChangeCalcAddr As ICalcAddress
    Private SortDoneCalcAddr As ICalcAddress
    Private CreateAnimDoneAddr As ICalcAddress
    Private CloneAnimDoneAddr As ICalcAddress
    Private SetAnimateAttr8Addr As ICalcAddress
    Private DoAttrDoneAddr As ICalcAddress


    Private AnimEoCProvider As IExtractOnCountProvider

    Public Property Scenario As Scenario

    Private Sub LaunchApp(Name As String)
        Const ProgToEdit = &H84BF
        Const RamCode = &H8100

        Dim AppCode() As Byte = {&HEF, &HD3, &H48, &HEF, &H51, &H4C}
        Dim NameBytes = System.Text.Encoding.ASCII.GetBytes(Name)
        _Calc.Break()
        _Calc.Memory.Write(ProgToEdit, NameBytes)
        _Calc.Memory.Write(RamCode, AppCode)
        _Calc.CPU.Halt = False
        _Calc.CPU.PC = RamCode
        _Calc.Run()
    End Sub

    Private _BreakpointMap As New Dictionary(Of Integer, String)

    Private Function AddBreakpoint(LabelName As String) As CalcAddress
        Dim Addr = _Asm.Labels(LabelName)
        Dim Page As Byte = Addr / &H10000
        Dim CalcAddr As New CalcAddress
        CalcAddr.Initialize(_Calc.Memory.Flash(ZeldaApp.Page.Index - Page), Addr And &HFFFF)
        _Calc.Breakpoints.Add(CalcAddr)

        _BreakpointMap.Add(Addr, LabelName.ToUpper)
        Return CalcAddr
    End Function

    Public Sub Initialize(MapIndex As Byte, StartX As Byte, StartY As Byte)
        If _IsLoading Then Exit Sub

        _IsLoading = True
        RaisePropertyChanged("IsLoading")
        Mouse.OverrideCursor = Cursors.Wait

        _Asm.CurrentDirectory = MapEditorControl.ZeldaFolder

        _Asm.InputFile = "zelda_all.asm"
        _Asm.OutputFile = Path.Combine(MapEditorControl.ZeldaFolder, "zelda.8xk")

        _Asm.IncludeDirectories.Add("defaults")
        _Asm.IncludeDirectories.Add("images")
        _Asm.IncludeDirectories.Add("maps")
        _Asm.IncludeDirectories.Add("scripts")

        _Asm.Defines.Add("_MAPEDITOR_TEST", 1)
        '_MAPEDITOR_SCENARIONAME, _MAPEDITOR_MAPINDEX, _MAPEDITOR_STARTX, _MAPEDITOR_STARTY)
        _Asm.Defines.Add("_MAPEDITOR_SCENARIONAME", Scenario.ScenarioName)
        _Asm.Defines.Add("_MAPEDITOR_MAPINDEX", MapIndex)

        _Asm.Defines.Add("_MAPEDITOR_STARTX", StartX)
        _Asm.Defines.Add("_MAPEDITOR_STARTY", StartY)

        Dim Output = _Asm.Assemble()
        Debug.Write(_Asm.StdOut.ReadAll())

        _Calc.LoadFile(MapEditorControl.RomPath)
        _Calc.LoadFile(_Asm.OutputFile)

        _Calc.Run()
        _Calc.Reset()

        _Calc.TurnCalcOn()
        _Calc.TurnCalcOn()

        DrawQueueAddr = _Asm.Labels("DRAW_QUEUE")
        DrawEntrySize = _Asm.Labels("DRAW_ENTRY_WIDTH")
        ScreenPosAddr = _Asm.Labels("SCREEN_XC")
        DrawEntryCountAddr = _Asm.Labels("DRAW_COUNT")
        MapDataAddr = _Asm.Labels("MAP_DATA")
        FlagsAddr = _Asm.Labels("GAME_FLAGS")
        CurrentMapAddr = _Asm.Labels("CURRENT_MAP")
        ScreenPosEAddr = _Asm.Labels("SCREEN_XC_E")
        AnimateArrayAddr = _Asm.Labels("ANIMATE_ARRAY")
        AnimateAmount = _Asm.Labels("ANIMATE_AMOUNT")
        TileTableAddr = _Asm.Labels("TILE_TABLE")
        LastIndexAddr = _Asm.Labels("LAST_INDEX")
        ResetGameAddr = _Asm.Labels("RESET_GAME")
        GameLoopAddr = _Asm.Labels("GAME_LOOP")

        Me.Scenario = Scenario

        For Each Define As String In SPASMHelper.Defines
            Dim DefineKey As String = Define.ToUpper()
            Debug.WriteLine("Processing: " & Define)
            If DefineKey Like "*_GFX?" Or DefineKey Like "*_GFX" Then
                Dim Address As UShort = _Asm.Labels(Define.ToUpper()) And &HFFFF
                If Not ImageMap.ContainsKey(Address) Then
                    ImageMap.Add(Address, SPASMHelper.Defines(Define))
                End If
            End If
        Next


        For Each App As ITIApplication In _Calc.Apps
            If App.Name Like "Zelda*" Then
                ZeldaApp = App
                Exit For
            End If
        Next

        ZeldaAppPage = ZeldaApp.Page.Index

        Memory = _Calc.Memory
        _Calc.Break()


        'AnimEoCProvider = New WabbitemuExtractOnCountProvider(_Calc, ZeldaApp.Page.Index)
        Dim Page1(16383) As Byte
        Dim TotalRead As UInteger
        Output.RemoteRead(Page1(0), 16384, TotalRead)
        AnimEoCProvider = New ByteArrayExtractOnCountProvider(Page1)

        AddBreakpoint("SORT_DONE")
        AddBreakpoint("PREPARE_CHANGE")
        AddBreakpoint("LOAD_MAP_COMPLETE")
        AddBreakpoint("CREATE_ANIM_DONE")
        AddBreakpoint("CLONE_ANIM_DONE")
        AddBreakpoint("SET_ANIMATE_ATTR8")
        AddBreakpoint("DO_ATTR_DONE")
        AddBreakpoint("CHANGE_DRAW_DONE")
        AddBreakpoint("GAME_LOOP")
        AddBreakpoint("MAP_EDITOR_CODE_DONE")

        Scenario.CacheMapHierarchy()

        AddHandler _Calc.Breakpoint, AddressOf Calc_Breakpoint

        LaunchApp("Zelda   ")
        _Calc.Run()
        _Calc.Visible = True

        Map = New MapData(Scenario, 0)

        DrawEntries = New ObservableCollection(Of ZDrawEntry)
        FrameProcessThread.Start()
    End Sub

    Public Sub New(Scenario As Scenario)
        _Calc = New Wabbitemu
        _Asm = New Z80Assembler

        Me.Scenario = Scenario
        Mouse.OverrideCursor = Cursors.Cross
    End Sub

    Private Function MapKey(Key As Key) As CalcKey?
        Select Case Key
            Case Input.Key.Down
                Return WabbitemuLib.CalcKey.KEY_DOWN
            Case Input.Key.Left
                Return WabbitemuLib.CalcKey.KEY_LEFT
            Case Input.Key.Right
                Return WabbitemuLib.CalcKey.KEY_RIGHT
            Case Input.Key.Up
                Return WabbitemuLib.CalcKey.KEY_UP
            Case Input.Key.LeftShift
                Return WabbitemuLib.CalcKey.KEY_2ND
            Case Input.Key.F1
                Return WabbitemuLib.CalcKey.KEY_YEQU
            Case Input.Key.LeftCtrl
                Return WabbitemuLib.CalcKey.KEY_ALPHA
            Case Else
                Return Nothing
        End Select
    End Function

    Public Function PressKey(Key As Key) As Boolean
        Dim CalcKey As CalcKey? = MapKey(Key)
        If CalcKey IsNot Nothing Then
            Try
                _Calc.Keypad.PressKey(CalcKey)
            Catch e As COMException
            End Try
            Return True
        Else
            Return False
        End If
    End Function

    Public Function ReleaseKey(Key As Key) As Boolean
        Dim CalcKey As CalcKey? = MapKey(Key)
        If CalcKey IsNot Nothing Then
            Dim Success = False
            Do Until Success
                Try
                    _Calc.Keypad.ReleaseKey(CalcKey)
                    Success = True
                Catch e As COMException
                End Try
            Loop
            Return True
        Else
            Return False
        End If
    End Function

    Public Sub Start()
        _Calc.Run()
    End Sub

    Public Sub Pause()
        _Calc.Break()
    End Sub

    Private Shared Function GetAZAnimBytes(ZAnim As AZAnim) As Byte()
        Dim Size = Marshal.SizeOf(ZAnim)
        Dim Data(Size - 1) As Byte
        Dim Ptr = Marshal.AllocHGlobal(Size)

        Marshal.StructureToPtr(ZAnim, Ptr, True)
        Marshal.Copy(Ptr, Data, 0, Size)
        Marshal.FreeHGlobal(Ptr)

        Return Data
    End Function

    Private Function CreateAnimFromBytes(Data() As Byte) As ZAnim
        Dim h = GCHandle.Alloc(Data, GCHandleType.Pinned)
        Dim ZAnim As AZAnim = Marshal.PtrToStructure(h.AddrOfPinnedObject, GetType(AZAnim))
        h.Free()
        ZAnim.AnimData -= TileTableAddr
        Dim ThisAnim = GetAZAnimBytes(ZAnim)
        Dim Anim As New ZAnim(Scenario.AnimDefs("GENERIC_ANIMATE"),
                              ZAnim.X, ZAnim.Y, New MemoryStream(ThisAnim))

        Dim Extract As New ExtractOnCount(ZAnim.Anim.Count, ZAnim.Anim.Pointer, AnimEoCProvider)
        Anim.ExtractOnCount = Extract
        Return Anim
    End Function

    Private LastIndex As Integer = -1

    Private _IsRetargeting = False
    Private _NewX As Byte
    Private _NewY As Byte
    Private _NewMapIndex As Integer

    Public Sub SetLocation(X As Byte, Y As Byte, MapIndex As Integer)
        _IsRetargeting = True
        _NewX = X
        _NewY = Y
        _NewMapIndex = MapIndex
    End Sub

    Private _IsRetargetMode As Boolean
    Public Sub EnableRetargetMode(Enable As Boolean)
        _IsRetargetMode = Enable
        RaisePropertyChanged("IsRetargetModeActive")

        If Enable Then
            Mouse.OverrideCursor = Cursors.Cross
        Else
            Mouse.OverrideCursor = Cursors.Arrow
        End If
    End Sub

    Public Property IsRetargetModeActive As Boolean
        Get
            Return _IsRetargetMode
        End Get
        Set(value As Boolean)
            EnableRetargetMode(value)
        End Set
    End Property

    Private Sub Calc_Breakpoint(Calc As IWabbitemu, Breakpoint As IBreakpoint)
        SyncLock _RunningLock
            If _Running Then
                Try
                    Dim Addr As Integer = CInt(Breakpoint.Address.Address) + ((ZeldaAppPage - Breakpoint.Address.Page.Index) << 16)
                    Select Case _BreakpointMap(Addr)
                        Case "GAME_LOOP"
                            If _IsRetargeting Then
                                Calc.CPU.L = _NewX
                                Calc.CPU.H = _NewY

                                Calc.CPU.D = _NewMapIndex
                                Calc.CPU.E = 8

                                Dim Code() As Byte = {&HCD, (ResetGameAddr And &HFF), ((ResetGameAddr >> 8) And &HFF)}
                                Calc.Memory.Write(GameLoopAddr, Code)
                            End If
                        Case "MAP_EDITOR_CODE_DONE"
                            If _IsRetargeting Then
                                Dim Code() As Byte = {0, 0, 0}
                                Calc.Memory.Write(GameLoopAddr, Code)
                                _IsRetargeting = False
                            End If
                        Case "PREPARE_CHANGE"
                        Case "SET_ANIMATE_ATTR8"
                            Dim Index = Calc.Memory.ReadByte(Calc.CPU.HLP)
                            If Index = 15 Then
                                LastIndex = Calc.Memory.ReadByte(LastIndexAddr)
                            Else
                                LastIndex = Index
                            End If
                        Case "DO_ATTR_DONE"
                            If LastIndex <> -1 Then
                                Dim Index = LastIndex
                                Dim AnimateData = Calc.Memory.Read(AnimateArrayAddr + Index * 10, 10)
                                Me.Dispatcher.InvokeAsync(
                                    Sub()
                                    If AnimateData(0) = 0 Then
                                        AnimatedTiles(Index) = Nothing
                                    Else
                                        Dim Anim = CreateAnimFromBytes(AnimateData)
                                        AnimatedTiles(Index) = Anim
                                    End If
                                End Sub)
                                LastIndex = -1
                            End If
                        Case "CLONE_ANIM_DONE", "CREATE_ANIM_DONE"
                            Dim LastIndex = Calc.Memory.ReadByte(LastIndexAddr)
                            Dim AnimateData = Calc.Memory.Read(AnimateArrayAddr + LastIndex * 10, 10)
                            Me.Dispatcher.InvokeAsync(
                                Sub()
                                Dim Anim = CreateAnimFromBytes(AnimateData)
                                Dim Added = False
                                For i = 0 To AnimatedTiles.Count - 1
                                    If AnimatedTiles(i) Is Nothing Then
                                        AnimatedTiles(i) = Anim
                                        Added = True
                                        Exit For
                                    End If
                                Next
                                If Not Added Then
                                    AnimatedTiles.Add(Anim)
                                End If
                            End Sub)
                        Case "LOAD_MAP_COMPLETE"
                            Dim AnimateArray As Byte()
                            AnimateArray = Calc.Memory.Read(AnimateArrayAddr, 10 * AnimateAmount)
                            Me.Dispatcher.InvokeAsync(
                                Sub()
                                Dim AnimatedTilesCollection As New ObservableCollection(Of ZAnim)
                                While AnimateArray.Length > 0
                                    If AnimateArray(0) > 0 Then
                                        Dim Anim = CreateAnimFromBytes(AnimateArray.Take(10).ToArray())
                                        Dim Added = False
                                        For i = 0 To AnimatedTilesCollection.Count - 1
                                            If AnimatedTilesCollection(i) Is Nothing Then
                                                AnimatedTilesCollection(i) = Anim
                                                Added = True
                                                Exit For
                                            End If
                                        Next
                                        If Not Added Then
                                            AnimatedTilesCollection.Add(Anim)
                                        End If
                                    End If
                                    AnimateArray = AnimateArray.Skip(10).ToArray()
                                End While
                                AnimatedTiles = AnimatedTilesCollection
                            End Sub)
                        Case "CHANGE_DRAW_DONE"
                            Debug.Print("Changing draw")
                            UpdateModel(Calc.Memory)
                            Debug.Print("X " & _EnemyScreen(0) & ", Y" & _EnemyScreen(2) & ", W " & _EnemyScreen(1) & ", H " & _EnemyScreen(3))
                        Case Else
                            UpdateModel(Calc.Memory)
                    End Select
                Catch e As COMException
                Finally
                    Calc.Step()
                    Calc.Run()
                End Try
            End If
        End SyncLock
    End Sub



    Private Sub FrameProcess()
        While ProcessEvent.WaitOne() AndAlso _Running
            Me.Dispatcher.InvokeAsync(
                Sub()
                    If Not _IsInitialized Then
                        _IsInitialized = True
                        RaisePropertyChanged("IsInitialized")

                        Mouse.OverrideCursor = Cursors.Arrow
                    End If
                    For Each Anim In AnimatedTiles
                        'If Anim.Bounds.IntersectsWith(ScreenBounds) Then
                        If Anim IsNot Nothing Then
                            Anim.ExtractOnCount.AdvanceFrame()
                        End If
                        'End If
                    Next

                    Dim FrameDrawEntries As New ObservableCollection(Of ZDrawEntry)
                    While DrawEntryRawData.Any()
                        Dim Entry = ZDrawEntry.FromData(DrawEntryRawData.Take(DrawEntrySize).ToArray())

                        If ImageMap.ContainsKey(Entry.Image) Then
                            Entry.Image = ImageMap(Entry.Image)
                        Else
                            Entry.Image = 0
                        End If
                        FrameDrawEntries.Add(Entry)
                        DrawEntryRawData = DrawEntryRawData.Skip(DrawEntrySize)
                    End While

                    DrawEntries = FrameDrawEntries
                    'SetValue(ScreenXProperty, ScreenX)
                    'SetValue(ScreenYProperty, ScreenY)

                    Screen = New Collection(Of Rect)(SplitRects(New Rect(ScreenX, ScreenY, 96, 64)))
                    EnemyScreen = New Collection(Of Rect)(SplitRects(New Rect(_EnemyScreen(0), _EnemyScreen(2), _EnemyScreen(1), _EnemyScreen(3))))

                    If _NewMap <> _CurrentMap Then
                        _CurrentMap = _NewMap
                        If Scenario.GetMap(_CurrentMap) IsNot Nothing Then
                            Map.Tileset = Scenario.GetMap(_CurrentMap).Tileset
                        End If
                        RaisePropertyChanged("SourceMap")
                    End If

                    For i = 0 To 255
                        If Map.TileData(i) <> MapRawData(i) Then
                            Map.TileData(i) = MapRawData(i)
                        End If
                    Next
                End Sub)
        End While
    End Sub

    Private DrawEntryRawData As IEnumerable(Of Byte)
    Private MapRawData() As Byte

    Public Sub UpdateModel(Memory As IMemoryContext)
        Dim DrawEntryCount = Memory.ReadByte(DrawEntryCountAddr)
        If DrawEntryCount > 0 Then
            DrawEntryRawData = Memory.Read(DrawQueueAddr, DrawEntryCount * DrawEntrySize)
        End If
        MapRawData = Memory.Read(MapDataAddr, 256)

        Dim ScreenPos = Memory.Read(ScreenPosAddr, 2)
        ScreenX = ScreenPos(0)
        ScreenY = ScreenPos(1)

        _EnemyScreen = Memory.Read(ScreenPosEAddr, 4)

        GameFlags = Memory.ReadByte(FlagsAddr)
        _NewMap = Memory.ReadByte(CurrentMapAddr)

        ProcessEvent.Set()
    End Sub

    Private _CurrentMap As Integer = -1
    Private _NewMap As Integer = -1
    Public ReadOnly Property SourceMap As MapData
        Get
            If _CurrentMap > -1 Then
                Dim MapData = Scenario.GetMap(_CurrentMap)
                Return MapData
            Else
                Return Nothing
            End If
        End Get
    End Property

    Public Sub StopSimulation()
        RemoveHandler _Calc.Breakpoint, AddressOf Calc_Breakpoint

        SyncLock _RunningLock
            _Running = False
            _Calc.Break()
        End SyncLock

        ' Wait a few frame's worth for final breakpoints
        Thread.Sleep(300)

        _Calc.Break()

        _Calc = Nothing
        _Asm = Nothing
        Memory = Nothing

        ProcessEvent.Set()

        FrameProcessThread.Join()

        ' Wait for Wabbitemu to be collected
        System.GC.Collect()
        System.GC.WaitForPendingFinalizers()
    End Sub

    '#Region "IDisposable Support"
    '    Private disposedValue As Boolean ' To detect redundant calls

    '    ' IDisposable
    '    Protected Overridable Sub Dispose(disposing As Boolean)
    '        If Not Me.disposedValue Then
    '            If disposing Then
    '                RemoveHandler _Calc.Breakpoint, AddressOf Calc_Breakpoint

    '                SyncLock _RunningLock
    '                    _Running = False
    '                    _Calc.Break()
    '                End SyncLock

    '                ' Wait a few frame's worth for final breakpoints
    '                Thread.Sleep(300)

    '                _Calc.Break()

    '                _Calc = Nothing
    '                _Asm = Nothing

    '                ProcessEvent.Set()

    '                FrameProcessThread.Join()
    '            End If

    '            ' TODO: free unmanaged resources (unmanaged objects) and override Finalize() below.
    '            ' TODO: set large fields to null.
    '        End If
    '        Me.disposedValue = True
    '    End Sub

    '    ' TODO: override Finalize() only if Dispose(ByVal disposing As Boolean) above has code to free unmanaged resources.
    '    'Protected Overrides Sub Finalize()
    '    '    ' Do not change this code.  Put cleanup code in Dispose(ByVal disposing As Boolean) above.
    '    '    Dispose(False)
    '    '    MyBase.Finalize()
    '    'End Sub

    '    ' This code added by Visual Basic to correctly implement the disposable pattern.
    '    Public Sub Dispose() Implements IDisposable.Dispose
    '        ' Do not change this code.  Put cleanup code in Dispose(disposing As Boolean) above.
    '        Dispose(True)
    '        GC.SuppressFinalize(Me)
    '    End Sub
    '#End Region

    Private Function SplitRect(Input As Rect, IsX As Boolean) As IEnumerable(Of Rect)
        If IsX Then
            If Input.Right > 256 Then
                Return {New Rect(Input.X, Input.Y, 256 - Input.X, Input.Height),
                        New Rect(0, Input.Y, Input.Right - 256, Input.Height)}
            End If
        Else
            If Input.Bottom > 256 Then
                Return {New Rect(Input.X, Input.Y, Input.Width, 256 - Input.Y),
                        New Rect(Input.X, 0, Input.Width, Input.Bottom - 256)}
            End If
        End If
        Return {Input}
    End Function

    Private Function SplitRects(Input As Rect) As ICollection(Of Rect)
        Dim XSplit = SplitRect(Input, True)
        Return XSplit.SelectMany(Function(r) SplitRect(r, False)).ToList()
    End Function


    Public Event PropertyChanged(sender As Object, e As PropertyChangedEventArgs) Implements INotifyPropertyChanged.PropertyChanged
    Private Sub RaisePropertyChanged(PropName As String)
        RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(PropName))
    End Sub

End Class
