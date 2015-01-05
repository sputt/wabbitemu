Imports System.Windows.Media.Animation
Imports System.Windows.Media.Effects
Imports System.Linq

Public Class TilesPanel

    Public Sub Tile_Clicked(sender As Object, args As Object)
        DataContext.CurrentLayer = LayerType.MapLayer
    End Sub
End Class

' Convert from TileImageSource -> TileSelection
Public Class TileSelectionConverter
    Implements IValueConverter

    Public Function Convert(value As Object, targetType As Type, parameter As Object, culture As Globalization.CultureInfo) As Object Implements IValueConverter.Convert
        If value Is Nothing Then Return Nothing
        Return AppModel.Instance.SelectedTileset.Tiles(value.TileIndex Mod 128)
    End Function

    Public Function ConvertBack(value As Object, targetType As Type, parameter As Object, culture As Globalization.CultureInfo) As Object Implements IValueConverter.ConvertBack
        If value Is Nothing Then Return Nothing

        Return New TileSelection(DirectCast(value, TileImageSource).Tileset, DirectCast(value, TileImageSource).Index)
    End Function
End Class

Public Class AnimatedTileSelectionConverter
    Implements IValueConverter

    Public Function Convert(value As Object, targetType As Type, parameter As Object, culture As Globalization.CultureInfo) As Object Implements IValueConverter.Convert
        If value Is Nothing Then Return Nothing

        Return DirectCast(value, TileSelection).AnimatedTileDef
    End Function

    Public Function ConvertBack(value As Object, targetType As Type, parameter As Object, culture As Globalization.CultureInfo) As Object Implements IValueConverter.ConvertBack
        If value Is Nothing Then Return Nothing
        Return New TileSelection(Nothing, DirectCast(value, ZDef))
    End Function
End Class