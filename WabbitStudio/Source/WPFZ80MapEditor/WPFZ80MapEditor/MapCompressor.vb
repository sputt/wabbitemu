Imports System.Linq
Imports System.Collections.Specialized

Public Class MapCompressor
    Public Const EndToken = &H3FF

    Private Class BitQueue
        Dim Bits As List(Of Boolean)

        Public Sub New()
            Bits = New List(Of Boolean)
        End Sub

        Public Sub New(Init As IEnumerable(Of Byte))
            Bits = New BitArray(Init.Reverse().ToArray()).Cast(Of Boolean)().Reverse().ToList()
        End Sub

        Function Dequeue(NumBit As UInteger) As UShort
            Dequeue = Bits.Take(NumBit).Aggregate(Of UShort)(0, Function(t, b) t * 2 + Convert.ToUInt16(b))
            Bits.RemoveRange(0, NumBit)
        End Function

        Sub Enqueue(Value As UShort, NumBit As UInteger)
            Bits.AddRange(New BitArray(BitConverter.GetBytes(Value).ToArray()).Cast(Of Boolean)().Take(NumBit).Reverse())
        End Sub

        Public Shared Function GetData(Queue As BitQueue) As Byte()
            Dim Result As New List(Of Byte)
            While Queue.Count(8) > 0
                Result.Add(Queue.Dequeue(8))
            End While
            If Queue.Count(1) > 0 Then
                While Queue.Count(1) < 8
                    Queue.Enqueue(0, 1)
                End While
                Result.Add(Queue.Dequeue(8))
            End If
            Return Result.ToArray()
        End Function

        ReadOnly Property Count(NumBit As UInteger) As Integer
            Get
                Return Bits.Count \ NumBit
            End Get
        End Property
    End Class

    Private Class CompDictionary
        Inherits Dictionary(Of UShort, List(Of Byte))

        Public Sub New()
            For i = 0 To 255
                Me.Add(i, {CByte(i)}.ToList())
            Next
            Me.Add(MapCompressor.EndToken, {CByte(0)}.ToList())
        End Sub

        Public Sub AddPattern(Code As UShort, Cap As UShort)
            Me.Add(Me.Count - 1, Me(Code))
            Me(Me.Count - 2) = Me(Me.Count - 2).Concat(Enumerable.Repeat(Me(Cap)(0), 1)).ToList()
        End Sub

        Public Function FindPattern(Data As IEnumerable(Of Byte)) As UShort
            Dim CurrentCode
            For CurrentCode = Me.Count - 2 To 0 Step -1
                If Me(CurrentCode).SequenceEqual(Data.Take(Me(CurrentCode).Count)) Then
                    Return CurrentCode
                End If
            Next
            Debug.Assert(0)
            Return MapCompressor.EndToken
        End Function
    End Class

    Public Shared Function Compress(Data As IEnumerable(Of Byte)) As Byte()

        Dim Input As New Queue(Of Byte)(Data)
        Dim Dict As New CompDictionary
        Dim Output As New BitQueue()

        Do While Input.Count > 0
            Dim CurrentCode As UShort = Dict.FindPattern(Input)
            Output.Enqueue(CurrentCode, 10)

            Dict(CurrentCode).ForEach(Sub(s) Input.Dequeue())
            If Input.Count > 0 Then Dict.AddPattern(CurrentCode, CUShort(Input.Peek()))
        Loop

        Output.Enqueue(MapCompressor.EndToken, 10)
        Return BitQueue.GetData(Output)
    End Function

    Public Shared Function Decompress(Data As IEnumerable(Of Byte)) As Byte()

        Dim Dict As New CompDictionary
        Dim Input As New BitQueue(Data)
        Dim Output As New List(Of Byte)

        Dim CurrentCode As UShort = Input.Dequeue(10)
        Do While Input.Count(10) > 0 And CurrentCode <> &H3FF
            Dim NextCode As UShort = Input.Dequeue(10)

            Dict.AddPattern(CurrentCode, NextCode)
            Output.AddRange(Dict(CurrentCode))

            CurrentCode = NextCode
        Loop

        Return Output.ToArray()
    End Function
End Class
