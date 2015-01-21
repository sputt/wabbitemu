Imports SPASM

Public Class SPASMHelper
    Private Shared Assembler As IZ80Assembler

    Public Shared Labels As New Dictionary(Of String, Integer)

    Public Class AssemblerDefines
        Implements IEnumerable

        Private _Map As New Dictionary(Of String, String)

        Public Sub Add(Name As String, Value As String)
            SPASMHelper.Assembler.Defines.Add(Name, Value)
            _Map.Add(Name, Value)
        End Sub

        Public Function GetEnumerator() As IEnumerator Implements IEnumerable.GetEnumerator
            Return _Map.Keys.GetEnumerator()
        End Function

        Default Public ReadOnly Property Fetch(Name As String)
            Get
                Return _Map(Name)
            End Get
        End Property

    End Class

    Public Shared Property Defines As AssemblerDefines

    Public Shared Sub Initialize(Path As String)
        Try
            Assembler = New Z80Assembler
        Catch e As System.Runtime.InteropServices.COMException
            Exit Sub
        End Try

        Defines = New AssemblerDefines()

        Assembler.CurrentDirectory = Path

        Assembler.Defines.Add("_MAPEDITOR", "1")
        Assembler.Defines.Add("TILE_TABLE", 0)

        Assembler.IncludeDirectories.Add(Path)
        Assembler.IncludeDirectories.Add(Path & "\Images")
        Assembler.IncludeDirectories.Add(Path & "\Defaults")
        Assembler.IncludeDirectories.Add(Path & "\Maps")
        Assembler.Assemble("#include ""objectdef.inc""")
        Dim StdOutput = Assembler.StdOut.ReadAll()

    End Sub

    Public Shared Function Eval(ByVal Expr As String) As Integer
        If Expr Is Nothing Or Expr = "" Then
            Expr = "0"
        End If

        Dim Result As Integer
        If Integer.TryParse(Expr, Result) Then
            Return Result
        Else
            Dim Bytes = Assemble(".dw " & Expr)
            Return CInt(BitConverter.ToUInt16(Bytes, 0))
        End If
    End Function

    Private Shared Sub Log(LogStr As String)
        Debug.Write(Now.ToFileTime & ": " & LogStr & vbCrLf)
    End Sub

    Public Shared Function Assemble(ByVal Code As String) As Byte()
        Dim Output = Assembler.Assemble(Code)
        'Dim StdOutput = Assembler.StdOut.ReadAll()
        'Debug.Write(StdOutput)

        Dim Data As New List(Of Byte)

        Dim st As New tagSTATSTG
        Output.Stat(st, 0)

        Dim Result() As Byte = New Byte(st.cbSize.QuadPart - 1) {}
        If st.cbSize.QuadPart > 0 Then
            Dim BytesRead As UInteger
            Output.RemoteRead(Result(0), st.cbSize.QuadPart, BytesRead)
        End If
        Return Result
    End Function

    Public Shared Function AssembleFile(FileName As String) As Byte()
        Dim FullPath = System.IO.Path.GetFullPath(FileName)
        Dim Bytes = Assemble("#include """ & FullPath & """")
        Labels.Clear()

        For Each Label In Assembler.Labels.Keys
            Labels.Add(Label, Assembler.Labels(Label))
        Next
        Return Bytes
    End Function
End Class
