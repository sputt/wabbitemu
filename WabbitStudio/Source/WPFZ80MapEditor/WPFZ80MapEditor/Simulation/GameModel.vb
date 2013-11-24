Imports System.Collections.ObjectModel
Imports WabbitemuLib
Imports SPASM
Imports System.Runtime.InteropServices
Imports System.Threading

Public Class GameModel

    Public Shared ReadOnly ScreeXProperty As DependencyProperty =
        DependencyProperty.Register("ScreeX", GetType(Byte), GetType(GameModel))
    Public Shared ReadOnly ScreeYProperty As DependencyProperty =
        DependencyProperty.Register("ScreenY", GetType(Byte), GetType(GameModel))

    Public ScreenX As Byte
    Public ScreenY As Byte

    ' Public PlayerX As Byte
    'Public PlayerY As Byte

    Public DrawEntries As New ObservableCollection(Of ZDrawEntry)

    Public ImageMap As New Dictionary(Of UShort, ImageSource)

    Private DrawQueueAddr As UShort
    Private DrawEntrySize As Byte
    Private ScreenPosAddr As UShort
    Private DrawEntryCountAddr As UShort
    Private Memory As IMemoryContext

    Public Sub New(Asm As Z80Assembler, Calc As IWabbitemu)
        DrawQueueAddr = Asm.Labels("DRAW_QUEUE")
        DrawEntrySize = Asm.Labels("DRAW_ENTRY_WIDTH")
        ScreenPosAddr = Asm.Labels("SCREEN_XC")
        DrawEntryCountAddr = Asm.Labels("DRAW_COUNT")
        Memory = Calc.Memory
    End Sub

    Private DrawEntryRawData() As Byte

    Const FPS = 25
    Private FrameTimeError As Integer = 0
    Private LastFrameTime As DateTime? = Nothing

    Public Sub UpdateModel(Asm As IZ80Assembler, Calc As IWabbitemu)
        Dim DrawEntryCount = Memory.ReadByte(DrawEntryCountAddr)

        DrawEntryRawData = Memory.Read(DrawQueueAddr, DrawEntryCount * DrawEntrySize)

        Dim ScreenPos = Memory.Read(ScreenPosAddr, 2)
        ScreenX = ScreenPos(0)
        ScreenY = ScreenPos(1)

    End Sub

End Class
