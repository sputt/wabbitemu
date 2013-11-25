Imports System.Collections.ObjectModel
Imports WabbitemuLib
Imports SPASM
Imports System.Runtime.InteropServices
Imports System.Threading

Public Class GameModel
    Inherits DependencyObject
    Implements IDisposable

    Public Shared ReadOnly ScreenXProperty As DependencyProperty =
        DependencyProperty.Register("ScreenX", GetType(Byte), GetType(GameModel))
    Public Shared ReadOnly ScreenYProperty As DependencyProperty =
        DependencyProperty.Register("ScreenY", GetType(Byte), GetType(GameModel))

    Public Shared ReadOnly DrawEntriesProperty As DependencyProperty =
        DependencyProperty.Register("DrawEntries", GetType(ObservableCollection(Of ZDrawEntry)), GetType(GameModel))

    Public Shared ReadOnly MapProperty As DependencyProperty =
        DependencyProperty.Register("Map", GetType(MapData), GetType(GameModel))

    Public ScreenX As Byte
    Public ScreenY As Byte

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

    Public ImageMap As New Dictionary(Of UShort, Integer)

    Private DrawQueueAddr As UShort
    Private DrawEntrySize As Byte
    Private ScreenPosAddr As UShort
    Private DrawEntryCountAddr As UShort
    Private MapDataAddr As UShort
    Private Memory As IMemoryContext

    Private FrameProcessThread As New Thread(AddressOf FrameProcess)
    Private ProcessEvent As New AutoResetEvent(False)

    Public Sub New(Asm As Z80Assembler, Calc As IWabbitemu)
        DrawQueueAddr = Asm.Labels("DRAW_QUEUE")
        DrawEntrySize = Asm.Labels("DRAW_ENTRY_WIDTH")
        ScreenPosAddr = Asm.Labels("SCREEN_XC")
        DrawEntryCountAddr = Asm.Labels("DRAW_COUNT")
        MapDataAddr = Asm.Labels("MAP_DATA")
        Memory = Calc.Memory

        Map = New MapData(0)

        DrawEntries = New ObservableCollection(Of ZDrawEntry)
        FrameProcessThread.Start()
    End Sub

    Private Sub FrameProcess()
        While ProcessEvent.WaitOne()
            Me.Dispatcher.Invoke(
                Sub()
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
                    SetValue(ScreenXProperty, ScreenX)
                    SetValue(ScreenYProperty, ScreenY)

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

    Public Sub UpdateModel(Asm As IZ80Assembler, Calc As IWabbitemu)
        Dim DrawEntryCount = Memory.ReadByte(DrawEntryCountAddr)

        DrawEntryRawData = Memory.Read(DrawQueueAddr, DrawEntryCount * DrawEntrySize)
        MapRawData = Memory.Read(MapDataAddr, 256)

        Dim ScreenPos = Memory.Read(ScreenPosAddr, 2)
        ScreenX = ScreenPos(0)
        ScreenY = ScreenPos(1)

        ProcessEvent.Set()
    End Sub

#Region "IDisposable Support"
    Private disposedValue As Boolean ' To detect redundant calls

    ' IDisposable
    Protected Overridable Sub Dispose(disposing As Boolean)
        If Not Me.disposedValue Then
            If disposing Then
                ' TODO: dispose managed state (managed objects).
            End If

            ' TODO: free unmanaged resources (unmanaged objects) and override Finalize() below.
            ' TODO: set large fields to null.
        End If
        Me.disposedValue = True
    End Sub

    ' TODO: override Finalize() only if Dispose(ByVal disposing As Boolean) above has code to free unmanaged resources.
    'Protected Overrides Sub Finalize()
    '    ' Do not change this code.  Put cleanup code in Dispose(ByVal disposing As Boolean) above.
    '    Dispose(False)
    '    MyBase.Finalize()
    'End Sub

    ' This code added by Visual Basic to correctly implement the disposable pattern.
    Public Sub Dispose() Implements IDisposable.Dispose
        ' Do not change this code.  Put cleanup code in Dispose(disposing As Boolean) above.
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub
#End Region

End Class
