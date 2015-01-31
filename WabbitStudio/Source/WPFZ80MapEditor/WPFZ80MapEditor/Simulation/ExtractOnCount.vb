Imports System.Windows.Media.Animation
Imports WabbitemuLib
Imports System.ComponentModel

Public Interface IExtractOnCountProvider
    Function Read(Addr As UShort) As ExtractOnCountEntry
End Interface

Public Class WabbitemuExtractOnCountProvider
    Implements IExtractOnCountProvider

    Private _Calc As IWabbitemu
    Private _Page As Byte

    Public Sub New(Calc As IWabbitemu, Page As Byte)
        _Calc = Calc
        _Page = Page
    End Sub

    Public Function Read(Addr As UShort) As ExtractOnCountEntry Implements IExtractOnCountProvider.Read
        Dim Count = _Calc.Memory.Flash(_Page).ReadByte(Addr - 1)
        Dim Value = _Calc.Memory.Flash(_Page).ReadWord(Addr)
        Return New ExtractOnCountEntry(Count, Value)
    End Function
End Class

Public Class ByteArrayExtractOnCountProvider
    Implements IExtractOnCountProvider

    Private _Data() As Byte

    Public Sub New(Data() As Byte)
        _Data = Data
    End Sub

    Public Function Read(Addr As UShort) As ExtractOnCountEntry Implements IExtractOnCountProvider.Read
        Dim Index = Addr Mod 16384
        Dim Count = _Data(Index - 1)
        Dim Value = _Data(Index) + _Data(Index + 1) * 256
        Return New ExtractOnCountEntry(Count, Value)
    End Function
End Class

Public Structure ExtractOnCountEntry
    Public Count As Byte
    Public Value As UShort

    Public Sub New(Count As Byte, Value As UShort)
        Me.Count = Count
        Me.Value = Value
    End Sub
End Structure

Public Class ExtractOnCount
    Implements INotifyPropertyChanged

    Private _StartAddr As UShort
    Private _CurrentAddr As UShort
    Public Property Count As Byte

    Private _Provider As IExtractOnCountProvider

    Public Sub New(Count As Byte, Addr As UShort, Provider As IExtractOnCountProvider)
        _StartAddr = Addr
        _CurrentAddr = Addr
        _Provider = Provider
        Me.Count = Count
    End Sub

    Private _CurrentValue As UShort = 0
    Public ReadOnly Property CurrentValue As Integer
        Get
            If _CurrentValue = 0 Then
                _CurrentValue = _Provider.Read(_CurrentAddr).Value
            End If
            Dim Result As Integer = _CurrentValue
            If _CurrentValue > &H7FFF Then
                Result = _CurrentValue - &H10000
            End If
            Return Result
        End Get
    End Property

    Private Function FindNextEntry(Addr As UShort, ByRef Entry As ExtractOnCountEntry) As UShort
        Dim ResultAddr = Addr + 3
        Entry = _Provider.Read(ResultAddr)
        If Entry.Count = 0 Then
            ResultAddr = Entry.Value + 1
            Entry = _Provider.Read(ResultAddr)
        End If
        Return ResultAddr
    End Function

    Public ReadOnly Property NextEntry As ExtractOnCountEntry
        Get
            Dim Entry As New ExtractOnCountEntry(0, 0)
            FindNextEntry(_CurrentAddr, Entry)
            Return Entry
        End Get
    End Property

    Public ReadOnly Property Entries As IList(Of ExtractOnCountEntry)
        Get
            Dim Result As New List(Of ExtractOnCountEntry)
            Dim Addr = _StartAddr
            Do
                Dim Entry As New ExtractOnCountEntry(0, 0)
                Addr = FindNextEntry(Addr, Entry)
                Result.Add(Entry)
            Loop Until Addr = _StartAddr
            Return Result
        End Get
    End Property

    Public Sub AdvanceFrame()
        Count = Count - 1
        If Count = 0 Then
            Dim Entry As New ExtractOnCountEntry(0, 0)
            _CurrentAddr = FindNextEntry(_CurrentAddr, Entry)
            _CurrentValue = Entry.Value
            Count = Entry.Count
            RaisePropertyChanged("CurrentValue")
        End If
    End Sub

    Private Sub RaisePropertyChanged(PropName As String)
        RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(PropName))
    End Sub

    Public Event PropertyChanged(sender As Object, e As PropertyChangedEventArgs) Implements INotifyPropertyChanged.PropertyChanged
End Class

Public Class EntryToStoryboardConverter
    Inherits OneWayConverter(Of ExtractOnCount, Storyboard)

    Private Const MILLISECONDS_PER_FRAME = 1000 / 39

    Public Overrides Function Convert(Extract As ExtractOnCount, Parameter As Object) As Storyboard
        Dim Entries = Extract.Entries
        Dim TotalFrames = Entries.Sum(Function(e) e.Count)
        Dim DurationInMillis = TotalFrames * MILLISECONDS_PER_FRAME

        Dim Anim As New Int32AnimationUsingKeyFrames()
        Anim.Duration = New Duration(TimeSpan.FromMilliseconds(DurationInMillis))

        Dim TimeSoFar = 0
        For Each Entry In Entries
            Dim Frame As New DiscreteInt32KeyFrame(Entry.Value,
                                                    KeyTime.FromTimeSpan(TimeSpan.FromMilliseconds(TimeSoFar * MILLISECONDS_PER_FRAME)))
            TimeSoFar += Entry.Count
            Anim.KeyFrames.Add(Frame)
        Next

        Anim.RepeatBehavior = RepeatBehavior.Forever

        Storyboard.SetTargetProperty(Anim, New PropertyPath(AnimatedTile.CurrentAnimValueProperty))

        ' Create a storyboard to apply the animation.
        Dim AnimStoryboard As New Storyboard()
        AnimStoryboard.Children.Add(Anim)
        Return AnimStoryboard
    End Function
End Class