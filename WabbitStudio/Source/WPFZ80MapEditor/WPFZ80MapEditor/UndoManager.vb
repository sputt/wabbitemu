
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

    ''' <summary>
    ''' Pushes a map save state to the undo stack
    ''' </summary>
    ''' <param name="MapToSave">Which map is going to be modified by the next operation</param>
    ''' <param name="TypesToSave">Which types in the map are going to be modified</param>
    ''' <remarks>Specifying types allows the UndoManager to selectively save parts of the map data</remarks>
    Public Shared Sub PushUndoState(MapToSave As MapData, TypesToSave As TypeFlags)
        If _CurSnapshotIndex = _Snapshots.Count - 1 Then
            _Snapshots.Add(MapToSave.Clone())
        Else
            _Snapshots.Insert(_CurSnapshotIndex + 1, MapToSave.Clone())
        End If
        _CurSnapshotIndex += 1
    End Sub

    Private Shared Sub RestoreFromSnapshot(Model As AppModel, IsUndo As Boolean)
        If IsUndo AndAlso _CurSnapshotIndex < 0 Then Exit Sub
        If Not IsUndo AndAlso _CurSnapshotIndex = _Snapshots.Count - 1 Then Exit Sub

        If Not IsUndo Then _CurSnapshotIndex += 1
        Dim OldSnapshotIndex = _CurSnapshotIndex
        Dim OldSelectedTile = Model.SelectedTile
        Dim MapDataToRestore = _Snapshots(_CurSnapshotIndex)
        If IsUndo Then _CurSnapshotIndex -= 1

        Dim OldMap = Model.Scenario.Maps.First(Function(m) m.ID = MapDataToRestore.ID)
        Dim OldMapWasSelected = (Model.SelectedMap Is OldMap)
        Dim Index = Model.Scenario.Maps.IndexOf(OldMap)
        Model.Scenario.Maps(Index) = MapDataToRestore

        _Snapshots(OldSnapshotIndex) = OldMap
        If OldMapWasSelected Then
            Model.SelectedMap = MapDataToRestore
        End If
        Model.SelectedTile = OldSelectedTile
    End Sub

    Public Shared Function CanUndo() As Boolean
        Return _CurSnapshotIndex <> -1
    End Function

    Public Shared Sub Undo(Model As AppModel)
        RestoreFromSnapshot(Model, True)
    End Sub

    Public Shared Function CanRedo() As Boolean
        Return _CurSnapshotIndex < _Snapshots.Count - 1
    End Function

    Public Shared Sub Redo(Model As AppModel)
        RestoreFromSnapshot(Model, False)
    End Sub

End Class
