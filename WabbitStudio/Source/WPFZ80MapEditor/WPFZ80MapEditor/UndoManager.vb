
Public Class UndoManager

    <Flags()>
    Public Enum TypeFlags As Integer
        Anims
        Objects
        Enemies
        Misc
    End Enum

    Public Shared Function TypeFlagFromType(Type As Type) As TypeFlags
        If Type Is GetType(ZAnim) Then
            Return TypeFlags.Anims
        ElseIf Type Is GetType(ZObject) Then
            Return TypeFlags.Objects
        ElseIf Type Is GetType(ZEnemy) Then
            Return TypeFlags.Enemies
        ElseIf Type Is GetType(ZMisc) Then
            Return TypeFlags.Misc
        Else
            Return 0
        End If
    End Function

    Private Shared _CurSnapshotIndex As Integer = -1
    Private Shared _Snapshots As New List(Of MapData)

    Public Shared Sub PushUndoState(MapToSave As MapData, TypesToSave As TypeFlags)
        If _CurSnapshotIndex = _Snapshots.Count - 1 Then
            _Snapshots.Add(MapToSave.Clone())
        Else
            _Snapshots.Insert(_CurSnapshotIndex + 1, MapToSave.Clone())
        End If
        _CurSnapshotIndex += 1
    End Sub

    Public Shared Function CanUndo() As Boolean
        Return _CurSnapshotIndex <> -1
    End Function

    Public Shared Sub Undo(Model As AppModel)
        If _CurSnapshotIndex > -1 Then

            Dim MapDataToRestore = _Snapshots(_CurSnapshotIndex)
            _CurSnapshotIndex -= 1

            Dim OldMap = Model.Scenario.Maps.First(Function(m) m.ID = MapDataToRestore.ID)
            Dim OldMapWasSelected = (Model.SelectedMap Is OldMap)
            Dim Index = Model.Scenario.Maps.IndexOf(OldMap)
            Model.Scenario.Maps(Index) = MapDataToRestore

            _Snapshots(_CurSnapshotIndex + 1) = OldMap

            If OldMapWasSelected Then
                Model.SelectedMap = MapDataToRestore
            End If
        End If
    End Sub

End Class
