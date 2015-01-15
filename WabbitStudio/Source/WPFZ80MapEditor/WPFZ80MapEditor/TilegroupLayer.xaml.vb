Public Class TilegroupLayer
    Inherits MapLayer

    Public Overrides ReadOnly Property LayerType As LayerType
        Get
            Return WPFZ80MapEditor.LayerType.TilegroupLayer
        End Get
    End Property



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
        TilegroupSelection = New TilegroupSelection(Map.Tileset, Result.ToList())
    End Sub
#End Region


    Private _StartGroupDrag As New Point
    Private _TileDataBackup As New List(Of Byte)

    Private Sub TilegroupSelectionShape_MouseLeftButtonDown(sender As Object, evt As MouseButtonEventArgs)
        _StartGroupDrag = Mouse.GetPosition(sender)

        If sender.CaptureMouse() Then
            _TileDataBackup.AddRange(Map.TileData)
            evt.Handled = True
        End If
    End Sub

    Private Sub TilegroupSelectionShape_MouseLeftButtonUp(sender As Object, evt As MouseButtonEventArgs)
        If Mouse.Captured Is sender Then
            sender.ReleaseMouseCapture()
            evt.Handled = True
        End If
    End Sub

        Private Sub TilegroupSelectionShape_MouseMove(sender As Object, evt As MouseEventArgs)
            If Mouse.Captured Is sender Then
                Dim Diff As Point = Mouse.GetPosition(sender) - _StartGroupDrag
                Dim Dx = Math.Round(Diff.X / 16.0)
                Dim Dy = Math.Round(Diff.Y / 16.0)

                Dim Di = Dy * 16 + Dx

                ' Assign the tiles
                Dim Entries As New List(Of TilegroupEntry)

                Dim Replacements As New Dictionary(Of Integer, Byte)
            TilegroupSelection.TilegroupEntries.ToList().ForEach(
                Sub(e)
                        If Replacements.ContainsKey(e.Index + Di) Then
                            Replacements.Remove(e.Index + Di)
                        End If
                        Map.TileData(e.Index + Di) = e.Tile.TileIndex
                        ' Put back the original data where it was
                        Replacements.Add(e.Index, _TileDataBackup(e.Index))
                        Entries.Add(New TilegroupEntry(e.Index + Di, e.Tile))
                    End Sub)

            For Each Index In Replacements.Keys.Except(Entries.Select(Function(e) e.Index))
                Map.TileData(Index) = Replacements(Index)
            Next

                TilegroupSelection = New TilegroupSelection(Map.Tileset, Entries)
                evt.Handled = True
            End If
        End Sub
    End Class
