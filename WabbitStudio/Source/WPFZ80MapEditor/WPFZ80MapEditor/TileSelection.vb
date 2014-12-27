
Public Class TileSelection
    Public Enum SelectionType
        Tile
        AnimatedTile
    End Enum

    Public Sub New(Index As Byte)
        Type = SelectionType.Tile
        TileIndex = Index
    End Sub

    Public Sub New(Anim As ZDef)
        Type = SelectionType.AnimatedTile
        AnimatedTileDef = Anim
    End Sub

    Public Property Type As SelectionType
    Public Property TileIndex As Byte
    Public Property AnimatedTileDef As ZDef
End Class
