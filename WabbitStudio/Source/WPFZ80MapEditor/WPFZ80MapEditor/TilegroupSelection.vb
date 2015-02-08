Imports System.Runtime.CompilerServices
Imports System.Collections.ObjectModel

<Serializable()>
Public Class TilegroupSelection

    Private Const MAP_WIDTH As Integer = 16

    <NonSerialized()>
    Private _Tileset As Tileset
    Public Property Tileset As Tileset
        Get
            Return _Tileset
        End Get
        Set(value As Tileset)
            _Tileset = value
        End Set
    End Property

    Private _TilegroupEntries As ICollection(Of TilegroupEntry)
    Public Property TilegroupEntries As ICollection(Of TilegroupEntry)
        Get
            Return _TilegroupEntries
        End Get
        Set(value As ICollection(Of TilegroupEntry))
            _TilegroupEntries = value
            RecalculateVertices()
        End Set
    End Property

    Public Sub RecalculateVertices()
        If _TilegroupEntries.Count = 0 Then Exit Sub

        Dim ResultList As New List(Of LineSegment)
        BuildSegmentList(_TilegroupEntries(0), _TilegroupEntries, New HashSet(Of TilegroupEntry), ResultList)

        Dim StartSegment As LineSegment = ResultList(0)
        ResultList.Remove(StartSegment)
        Dim CurSegment = FindConnectedSegment(StartSegment, ResultList)
        If CurSegment Is Nothing Then Exit Sub

        ' Create the seed point
        Dim RawVertices As New List(Of Point)
        RawVertices.Add(StartSegment.GetNonConnectedPoint(CurSegment))
        Do
            ResultList.Remove(CurSegment)
            Dim NextSegment = FindConnectedSegment(CurSegment, ResultList)
            If NextSegment IsNot Nothing Then
                RawVertices.Add(CurSegment.GetNonConnectedPoint(NextSegment))
            End If
            CurSegment = NextSegment
        Loop Until CurSegment Is Nothing OrElse CurSegment.IsConnectedTo(StartSegment)

        If CurSegment IsNot Nothing Then RawVertices.Add(CurSegment.GetNonConnectedPoint(StartSegment))

        Dim MinX As Integer = RawVertices.Select(Function(v) v.X).Min()
        Dim MinY As Integer = RawVertices.Select(Function(v) v.Y).Min()

        _MapOffset = New Point(MinX, MinY)

        _Vertices.AddRange(RawVertices.Select(Function(v)
                                                  v.Offset(-MinX, -MinY)
                                                  Return v
                                              End Function))
    End Sub

    Public ReadOnly Property Bounds As Rect
        Get
            Dim MinX As Integer = _Vertices.Select(Function(v) v.X).Min()
            Dim MinY As Integer = _Vertices.Select(Function(v) v.Y).Min()
            Dim MaxX As Integer = _Vertices.Select(Function(v) v.X).Max()
            Dim MaxY As Integer = _Vertices.Select(Function(v) v.Y).Max()

            Return New Rect(0, 0, MaxX - MinX, MaxY - MinY)
        End Get
    End Property


    Private ReadOnly _Vertices As New List(Of Point)

    Public Sub New()
    End Sub

    Public Sub New(Tileset As Tileset, Entries As IEnumerable(Of TilegroupEntry))
        Me.Tileset = Tileset
        Me.TilegroupEntries = Entries
    End Sub

    Private _MapOffset As Point
    Public ReadOnly Property MapOffset As Point
        Get
            Return _MapOffset
        End Get
    End Property

    Private Function FindConnectedSegment(Segment As LineSegment, Segments As IEnumerable(Of LineSegment)) As LineSegment
        Return Segments.FirstOrDefault(Function(s) s.IsConnectedTo(Segment))
    End Function

    Private Class LineSegment
        Public Property P1 As Point
        Public Property P2 As Point

        Public Sub New(P1 As Point, P2 As Point)
            Me.P1 = P1
            Me.P2 = P2
        End Sub

        Public Function IsConnectedTo(Other As LineSegment) As Boolean
            Return P1 = Other.P1 Or P2 = Other.P2 Or P1 = Other.P2 Or P2 = Other.P1
        End Function

        Public Function GetNonConnectedPoint(Other As LineSegment) As Point
            Return If(P1 = Other.P1 Or P1 = Other.P2, P2, P1)
        End Function
    End Class

    Private Sub BuildSegmentList(
        Current As TilegroupEntry,
        Entries As IEnumerable(Of TilegroupEntry),
        Visited As ISet(Of TilegroupEntry),
        ByRef Result As List(Of LineSegment))

        Visited.Add(Current)
        For Each Dir In [Enum].GetValues(GetType(Dir))
            Dim Entry = NextEntry(Current, Entries, Dir)
            If Not Visited.Contains(Entry) Then
                If Entry Is Nothing Then
                    Result.Add(IndexToSegment(Current.Index, Dir))
                Else
                    BuildSegmentList(Entry, Entries, Visited, Result)
                End If
            End If
        Next
    End Sub

    Private Function NextEntry(Current As TilegroupEntry, Entries As IEnumerable(Of TilegroupEntry), Dir As Dir) As TilegroupEntry
        Dim Index = Current.Index
        Dim Row As Integer = Math.Floor(Current.Index / MAP_WIDTH), Col As Integer = Current.Index Mod MAP_WIDTH

        Dim NewIndex As Integer = -1
        Select Case Dir
            Case Dir.Left
                NewIndex = If(Col = 0, -1, Index - 1)
            Case Dir.Right
                NewIndex = If(Col = MAP_WIDTH - 1, -1, Index + 1)
            Case Dir.Up
                NewIndex = If(Row = 0, -1, Index - MAP_WIDTH)
            Case Dir.Down
                NewIndex = If(Row = MAP_WIDTH - 1, -1, Index + MAP_WIDTH)
        End Select
        Return Entries.FirstOrDefault(Function(t) t.Index = NewIndex)
    End Function

    Private Function IndexToSegment(Index As Integer, Dir As Dir) As LineSegment
        Dim Row As Integer = Math.Floor(Index / MAP_WIDTH), Col As Integer = Index Mod MAP_WIDTH

        Select Case Dir
            Case Dir.Left
                Return New LineSegment(IndexToPoint(Col, Row), IndexToPoint(Col, Row + 1))
            Case Dir.Right
                Return New LineSegment(IndexToPoint(Col + 1, Row), IndexToPoint(Col + 1, Row + 1))
            Case Dir.Up
                Return New LineSegment(IndexToPoint(Col, Row), IndexToPoint(Col + 1, Row))
            Case Dir.Down
                Return New LineSegment(IndexToPoint(Col, Row + 1), IndexToPoint(Col + 1, Row + 1))
            Case Else
                Return Nothing
        End Select
    End Function

    Private Function IndexToPoint(Col As Integer, Row As Integer) As Point
        Return New Point(Col * 16, Row * 16)
    End Function

    Private Enum Dir
        Left
        Right
        Up
        Down
    End Enum

    Public ReadOnly Property Vertices As ICollection(Of Point)
        Get
            Return _Vertices
        End Get
    End Property
End Class

<Serializable()>
Public Class TilegroupEntry
    ''' <summary>
    ''' Index into the map data
    ''' </summary>
    Public Property Index As Integer
    ''' <summary>
    ''' Type of tile to place at the given index
    ''' </summary>
    Public Property Tile As TileSelection

    Public Sub New()
    End Sub

    Public Sub New(Index As Integer, Tile As TileSelection)
        Me.Index = Index
        Me.Tile = Tile
    End Sub

    Public Sub New(Index As Integer, TileIndex As Byte)
        Me.Index = Index
        Me.Tile = New TileSelection(Nothing, TileIndex)
    End Sub

    Public Sub New(Index As Integer, Anim As ZDef)
        Me.Index = Index
        Me.Tile = New TileSelection(Nothing, Anim)
    End Sub

    Public Overrides Function Equals(obj As Object) As Boolean
        If obj Is Nothing OrElse Not Me.GetType() Is obj.GetType() Then
            Return False
        End If

        Dim Other As TilegroupEntry = obj
        Return Index = Other.Index And Tile.Type = Other.Tile.Type And
            Tile.Tileset Is Other.Tile.Tileset And Tile.TileIndex = Other.Tile.TileIndex And Tile.AnimatedTileDef Is Other.Tile.AnimatedTileDef
    End Function

    Public Overrides Function GetHashCode() As Integer
        Return Index Xor Tile.TileIndex
    End Function
End Class

<Serializable()>
Public Class TilegroupEntryCollection
    Inherits List(Of TilegroupEntry)
End Class

Public Class SegmentConverter
    Inherits OneWayConverter(Of ICollection(Of Point), PathSegmentCollection)

    Public Overrides Function Convert(Vertices As ICollection(Of Point), parameter As Object) As PathSegmentCollection
        Dim PathSegments As New List(Of PathSegment)

        For Each Vertex In Vertices.Skip(1)
            Dim Line As New LineSegment(Vertex, True)
            PathSegments.Add(Line)
        Next
        Return New PathSegmentCollection(PathSegments)
    End Function
End Class