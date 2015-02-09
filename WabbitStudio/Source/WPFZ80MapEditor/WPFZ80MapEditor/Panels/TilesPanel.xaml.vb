Imports System.Windows.Media.Animation
Imports System.Windows.Media.Effects
Imports System.Linq

Public Class TilesPanel
    Friend Shared Model As AppModel

    Public Sub New()
        InitializeComponent()
        AddHandler DataContextChanged, AddressOf OnDataContextChanged
    End Sub

    Private Sub OnDataContextChanged(ByVal sender As Object, ByVal e As DependencyPropertyChangedEventArgs)
        Model = DataContext
    End Sub

    Public Sub Tile_Clicked(sender As Object, args As Object)
        If DataContext.CurrentLayer <> LayerType.MapLayer And DataContext.CurrentLayer <> LayerType.TilegroupLayer Then
            DataContext.CurrentLayer = LayerType.MapLayer
        End If
    End Sub
End Class

' Convert from TileImageSource -> TileSelection
Public Class TileSelectionConverter
    Implements IValueConverter

    Public Function Convert(value As Object, targetType As Type, parameter As Object, culture As Globalization.CultureInfo) As Object Implements IValueConverter.Convert
        If value Is Nothing Then Return Nothing
        If TilesPanel.Model Is Nothing Then
            Return Nothing
        End If

        Return TilesPanel.Model.SelectedTileset.Tiles(value.TileIndex Mod 128)
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