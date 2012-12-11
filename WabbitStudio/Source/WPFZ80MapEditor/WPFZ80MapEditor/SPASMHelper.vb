Imports SPASM

Public Class SPASMHelper
    Public Shared Assembler As IZ80Assembler

    Public Shared Labels As New Dictionary(Of String, Integer)

    Shared Sub New()
        Try
            Assembler = New Z80Assembler
        Catch e As System.Runtime.InteropServices.COMException
            Exit Sub
        End Try

        Assembler.AddDefine("_MAPEDITOR")
        Assembler.AddIncludeDirectory(Environment.CurrentDirectory)
        Assembler.AddIncludeDirectory(Environment.CurrentDirectory & "\Scenario")
        Assembler.Assemble("#include ""objectdef.inc""")
    End Sub

    Public Shared Function Eval(ByVal Expr As String) As Integer
        If Expr Is Nothing Then
            Expr = "0"
        End If
        Dim Bytes = Assemble(".dw " & Expr)
        Return CInt(BitConverter.ToUInt16(Bytes, 0))
    End Function

    Public Shared Function Assemble(ByVal Code As String) As Byte()
        If Assembler.Assemble(Code) <> 0 Then
            Throw New Exception("Assembling this object failed!")
        End If

        Dim Data As New List(Of Byte)

        Dim st As New tagSTATSTG
        Assembler.Output.Stat(st, 0)

        Dim Result() As Byte = New Byte(st.cbSize.QuadPart - 1) {}
        If st.cbSize.QuadPart > 0 Then
            Dim BytesRead As UInteger
            Assembler.Output.RemoteRead(Result(0), st.cbSize.QuadPart, BytesRead)
        End If
        Return Result
    End Function

    Public Shared Function AssembleFile(FileName As String) As Byte()
        Dim FullPath = System.IO.Path.GetFullPath(FileName)
        Dim Bytes = Assemble("#include """ & FullPath & """")
        Labels.Clear()
        For Each Label As IZ80Label In Assembler.Labels
            Labels.Add(Label.Name, Label.Value)
        Next
        Return Bytes
    End Function
End Class
