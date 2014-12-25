Imports System.IO
Imports System.Text.RegularExpressions
Imports System.Threading.Tasks

Partial Class Scenario
    Private Async Function LoadDefs(FileName As String, Dictionary As Dictionary(Of String, ZDef), InstanceType As Type) As Task
        'Dim Rx As New Regex(
        '    "^;(?<Name>[a-zA-Z][a-zA-Z ]+) - (?<Description>.+)\s*" & _
        '    "(^; (?<ArgName>\w+) - (?<ArgDesc>.+)\s*)*" & _
        '    "(^;Properties\s*)?" & _
        '    "(^; [a-zA-Z_]+ = .+\s*)*" & _
        '    "#macro (?<MacroName>[a-z0-9_]+).*\s*$", RegexOptions.Multiline Or RegexOptions.Compiled)

        Dim Rx As New ZDefRegex()

        Dim Stream = New StreamReader(FileName)
        Dim Matches = Rx.Matches(Await Stream.ReadToEndAsync())
        Stream.Close()

        For Each Match As Match In Matches
            Dim Groups = Match.Groups
            Dim Def As New ZDef(Groups("Name").Value, Groups("MacroName").Value.ToUpper(), Groups("Description").Value, InstanceType)

            For i = 0 To Groups("ArgName").Captures.Count - 1
                Def.AddArg(Groups("ArgName").Captures(i).Value.ToUpper(), Groups("ArgDesc").Captures(i).Value, Images)
            Next

            Dictionary.Add(Def.Macro, Def)
        Next
    End Function
End Class
