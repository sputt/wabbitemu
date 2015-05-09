Imports System.Collections.ObjectModel

Public Class TilegroupLayer
    Inherits MapLayer
    Implements IMapLayer

    ''' <summary>
    ''' Whether or not the selection is floating.  When floating, the delete key will simply remove the selection without applying.
    ''' When not floating, the delete key will fill the selection with the selected tile
    ''' </summary>
    ''' <remarks></remarks>
    Public Property Floating As Boolean
        Get
            Return GetValue(FloatingProperty)
        End Get

        Set(ByVal value As Boolean)
            SetValue(FloatingProperty, value)
        End Set
    End Property

    Public Shared ReadOnly FloatingProperty As DependencyProperty = _
                           DependencyProperty.Register("Floating", _
                           GetType(Boolean), GetType(TilegroupLayer), _
                           New PropertyMetadata(False))

    Public Sub New()
        InitializeComponent()
    End Sub

    Public Overrides Sub DeselectAll()
        TilegroupSelection = Nothing
    End Sub

    Public Property TilegroupSelection As TilegroupSelection
        Get
            Return GetValue(TilegroupSelectionProperty)
        End Get

        Set(ByVal value As TilegroupSelection)
            SetValue(TilegroupSelectionProperty, value)
        End Set
    End Property

    Public Shared ReadOnly TilegroupSelectionProperty As DependencyProperty = _
                           DependencyProperty.Register("TilegroupSelection", _
                           GetType(TilegroupSelection), GetType(TilegroupLayer), _
                           New PropertyMetadata(Nothing))

    Public Property SourceTile As TileSelection
        Get
            Return GetValue(SourceTileProperty)
        End Get

        Set(ByVal value As TileSelection)
            SetValue(SourceTileProperty, value)
        End Set
    End Property

    Public Shared ReadOnly SourceTileProperty As DependencyProperty = _
                           DependencyProperty.Register("SourceTile", _
                           GetType(TileSelection), GetType(TilegroupLayer), _
                           New PropertyMetadata(Nothing))

#Region "Canvas events"
    Private _StartDrag As New Point
    Private _WasStartSelected As Boolean = False

    Protected Sub ObjectCanvas_MouseLeftButtonDown(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs)
        SelectionRect.Width = 0
        SelectionRect.Height = 0

        _StartDrag = Mouse.GetPosition(ObjectCanvas)

        _WasStartSelected = False
        If TilegroupSelection IsNot Nothing AndAlso TilegroupSelection.TilegroupEntries IsNot Nothing Then
            Dim PotentialSel = TilegroupEntryFromPoint(_StartDrag)
            _WasStartSelected = TilegroupSelection.TilegroupEntries.Contains(PotentialSel)
        End If

        If sender.CaptureMouse() Then
            SelectionRect.Visibility = Windows.Visibility.Visible

            TilegroupLayer.Focus()
            e.Handled = True
        End If
    End Sub

    Protected Sub ObjectCanvas_MouseLeftButtonUp(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs)
        If Mouse.Captured Is sender Then
            sender.ReleaseMouseCapture()
            SelectionRect.Visibility = Windows.Visibility.Hidden

            Dim SelRect As New Rect
            SelRect.X = Canvas.GetLeft(SelectionRect)
            SelRect.Y = Canvas.GetTop(SelectionRect)
            SelRect.Width = SelectionRect.Width
            SelRect.Height = SelectionRect.Height

            SelectObjectsInRect(SelRect)
            TilegroupLayer.Focus()
            e.Handled = True
        End If
    End Sub

    Protected Sub ObjectCanvas_MouseMove(sender As System.Object, e As System.Windows.Input.MouseEventArgs)
        If Mouse.Captured Is sender Then
            Dim CurPoint = Mouse.GetPosition(ObjectCanvas)
            CurPoint.X = Math.Max(0, Math.Min(256, CurPoint.X))
            CurPoint.Y = Math.Max(0, Math.Min(256, CurPoint.Y))

            Canvas.SetLeft(SelectionRect, Math.Min(CurPoint.X, _StartDrag.X))
            Canvas.SetTop(SelectionRect, Math.Min(CurPoint.Y, _StartDrag.Y))
            SelectionRect.Width = Math.Abs(CurPoint.X - _StartDrag.X)
            SelectionRect.Height = Math.Abs(CurPoint.Y - _StartDrag.Y)

            Dim SelRect As New Rect
            SelRect.X = Canvas.GetLeft(SelectionRect)
            SelRect.Y = Canvas.GetTop(SelectionRect)
            SelRect.Width = SelectionRect.Width
            SelRect.Height = SelectionRect.Height

            SelectObjectsInRect(SelRect)

            e.Handled = True
        End If
    End Sub

    Private Function TilegroupEntryFromPoint(Point As Point) As TilegroupEntry
        Dim Row As Integer = Math.Floor(Point.Y / 16)
        Dim Col As Integer = Math.Floor(Point.X / 16)

        Dim Index = Row * 16 + Col
        Return New TilegroupEntry(Index, Map.TileData(Index))
    End Function

    Protected Sub SelectObjectsInRect(SelRect As Rect)
        Dim Entries As New List(Of TilegroupEntry)

        For Y As Double = Math.Floor(SelRect.Top / 16.0) To Math.Ceiling(SelRect.Bottom / 16.0) - 1
            For X As Double = Math.Floor(SelRect.Left / 16.0) To Math.Ceiling(SelRect.Right / 16.0) - 1
                Entries.Add(TilegroupEntryFromPoint(New Point(X * 16.0, Y * 16.0)))
            Next
        Next

        Dim Result As IEnumerable(Of TilegroupEntry) = Entries
        If Keyboard.IsKeyDown(Key.LeftCtrl) Then
            If TilegroupSelection IsNot Nothing Then
                If _WasStartSelected Then
                    Result = TilegroupSelection.TilegroupEntries.Except(Entries)
                Else
                    Result = TilegroupSelection.TilegroupEntries.Union(Entries)
                End If
            End If
        End If

        Floating = False
        TilegroupSelection = New TilegroupSelection(Map.Tileset, Result.ToList())
    End Sub
#End Region


    Private _StartGroupDrag As New Point
    Private _TileDataBackup As New List(Of Byte)

    Private Sub TilegroupSelectionShape_MouseLeftButtonDown(sender As Object, evt As MouseButtonEventArgs)
        If Keyboard.IsKeyDown(Key.LeftCtrl) Then
            Exit Sub
        End If

        _StartGroupDrag = Mouse.GetPosition(sender)

        If sender.CaptureMouse() Then
            UndoManager.PushUndoState(Map, UndoManager.TypeFlags.Anims)

            TilegroupLayer.Focus()
            If Not Floating Then
                _TileDataBackup.Clear()
                _TileDataBackup.AddRange(Map.TileData)
                If SourceTile IsNot Nothing AndAlso SourceTile.Type = TileSelection.SelectionType.Tile AndAlso Not Floating Then
                    TilegroupSelection.TilegroupEntries.ToList().ForEach(
                        Sub(e)
                        _TileDataBackup(e.Index) = SourceTile.TileIndex
                    End Sub)
                End If
            End If
            evt.Handled = True
        End If
    End Sub

    Private Sub TilegroupSelectionShape_MouseLeftButtonUp(sender As Object, evt As MouseButtonEventArgs)
        If Mouse.Captured Is sender Then
            sender.ReleaseMouseCapture()

            evt.Handled = True
        End If
    End Sub

    Private Sub ApplyTilegroupSelection(IndexOffset As Integer)
        Dim Entries As New List(Of TilegroupEntry)

        Dim Replacements As New Dictionary(Of Integer, Byte)
        TilegroupSelection.TilegroupEntries.ToList().ForEach(
            Sub(e)
                If e.Index + IndexOffset >= 0 And e.Index + IndexOffset < Map.TileData.Count Then
                    Map.TileData(e.Index + IndexOffset) = e.Tile.TileIndex
                End If
                If IndexOffset <> 0 Then
                    ' Put back the original data where it was
                    If e.Index >= 0 And e.Index < _TileDataBackup.Count Then
                        Replacements(e.Index) = _TileDataBackup(e.Index)
                    End If
                End If
                Entries.Add(New TilegroupEntry(e.Index + IndexOffset, e.Tile))
            End Sub)

        For Each Index In Replacements.Keys.Except(Entries.Select(Function(e) e.Index))
            Map.TileData(Index) = Replacements(Index)
        Next

        If IndexOffset <> 0 Then
            TilegroupSelection = New TilegroupSelection(Map.Tileset, Entries)
        End If
    End Sub


    Private Sub TilegroupSelectionShape_MouseMove(sender As Object, evt As MouseEventArgs)
        If Mouse.Captured Is sender Then
            Dim Pos = Mouse.GetPosition(sender)
            Debug.Print(TilegroupSelection.MapOffset.X)
            Pos.X = Math.Min(255, Math.Max(-TilegroupSelection.Bounds.X, Pos.X))

            Dim Diff As Point = Pos - _StartGroupDrag
            Dim Dx = Math.Round(Diff.X / 16.0)
            If (Dx < 0 And TilegroupSelection.MapOffset.X = 0) Or (Dx > 0 And TilegroupSelection.MapOffset.X + TilegroupSelection.Bounds.Width = 256) Then
                Dx = 0
            End If

            Dim Dy = Math.Round(Diff.Y / 16.0)
            If (Dy < 0 And TilegroupSelection.MapOffset.Y = 0) Or (Dy > 0 And TilegroupSelection.MapOffset.Y + TilegroupSelection.Bounds.Height = 256) Then
                Dy = 0
            End If

            Dim Di = Dy * 16 + Dx
            If Di <> 0 Then
                Floating = True
            End If

            ' Assign the tiles
            ApplyTilegroupSelection(Di)

            evt.Handled = True
        End If
    End Sub


    Private Sub TilegroupLayer_KeyDown_1(sender As Object, evt As KeyEventArgs)
        If evt.Key = Key.Delete AndAlso TilegroupSelection IsNot Nothing Then
            UndoManager.PushUndoState(Map, 0)
            If Floating Then
                For i = 0 To 255
                    Map.TileData(i) = _TileDataBackup(i)
                Next
                TilegroupSelection = Nothing
            Else
                If SourceTile IsNot Nothing AndAlso SourceTile.Type = TileSelection.SelectionType.Tile Then
                    TilegroupSelection.TilegroupEntries.ToList().ForEach(
                        Sub(e)
                            Map.TileData(e.Index) = SourceTile.TileIndex
                        End Sub)
                End If
                TilegroupSelection = Nothing
            End If
            evt.Handled = True
        ElseIf evt.Key = Key.Escape Then
            TilegroupSelection = Nothing
        End If
    End Sub

    Private Sub Copy_CanExecute(sender As Object, e As CanExecuteRoutedEventArgs)
        If Active AndAlso TilegroupSelection IsNot Nothing Then
            e.CanExecute = True
            e.Handled = True
        End If
    End Sub

    Private Sub Copy_Executed(sender As Object, e As ExecutedRoutedEventArgs)
        If Active Then
            Dim ClipboardTilegroup = New TilegroupSelection()
            ClipboardTilegroup.TilegroupEntries = New TilegroupEntryCollection()
            For Each Entry In TilegroupSelection.TilegroupEntries
                ClipboardTilegroup.TilegroupEntries.Add(Entry)
            Next

            Clipboard.SetData(GetType(TilegroupSelection).FullName, ClipboardTilegroup)
            e.Handled = True
        End If
    End Sub

    Overrides Function CanPaste() As Boolean
        Return Clipboard.ContainsData(GetType(TilegroupSelection).FullName)
    End Function

    Public Overrides Sub Paste()
        Dim Data As TilegroupSelection = Clipboard.GetData(GetType(TilegroupSelection).FullName)
        Data.Tileset = Map.Tileset
        Data.RecalculateVertices()
        Floating = True
        TilegroupSelection = Data

        _TileDataBackup.Clear()
        _TileDataBackup.AddRange(Map.TileData)
        ApplyTilegroupSelection(0)
    End Sub
End Class
