Imports System.IO
Imports System.Text.RegularExpressions

Partial Class Scenario
    Private Sub LoadDefs(FileName As String, Defs As DependencyProperty, InstanceType As Type)
        Dim Rx As New Regex(
            "^;(?<Name>[a-zA-Z][a-zA-Z ]+) - (?<Description>.+)\s*" & _
            "(^; (?<ArgName>\w+) - (?<ArgDesc>.+)\s*)*" & _
            "(^;Properties\s*)?" & _
            "(^; [a-zA-Z_]+ = .+\s*)*" & _
            "#macro (?<MacroName>[a-z0-9_]+).*\s*$", RegexOptions.Multiline Or RegexOptions.Compiled)

        Dim Stream = New StreamReader(FileName)
        Dim Matches = Rx.Matches(Stream.ReadToEnd())
        Stream.Close()

        Dim Dictionary As ObservableDictionary(Of String, ZDef) = GetValue(Defs)

        For Each Match As Match In Matches
            Dim Groups = Match.Groups
            Dim Def As New ZDef(Groups("Name").Value, Groups("MacroName").Value, Groups("Description").Value, InstanceType)

            For i = 0 To Groups("ArgName").Captures.Count - 1
                Def.AddArg(Groups("ArgName").Captures(i).Value, Groups("ArgDesc").Captures(i).Value, Images)
            Next

            Dictionary.Add(Def.Macro, Def)
        Next
    End Sub
End Class
