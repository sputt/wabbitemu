
<Serializable()>
Public Class TileSelection
    Public Enum SelectionType
        Tile
        AnimatedTile
    End Enum

    Public Sub New()
    End Sub

    Public Sub New(Tileset As Tileset, Index As Byte)
        Me.Tileset = Tileset
        Type = SelectionType.Tile
        TileIndex = Index
    End Sub

    Public Sub New(Tileset As Tileset, Anim As ZDef)
        Me.Tileset = Tileset
        Type = SelectionType.AnimatedTile
        AnimatedTileDef = Anim
    End Sub

    Public Property Type As SelectionType
    Public Property TileIndex As Byte
    Public Property AnimatedTileDef As ZDef

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
End Class
