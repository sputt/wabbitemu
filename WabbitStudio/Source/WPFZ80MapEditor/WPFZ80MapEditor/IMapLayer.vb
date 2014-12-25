Public Interface IMapLayer
    WriteOnly Property Active As Boolean

    ReadOnly Property LayerType As LayerType

    Sub DeselectAll()
End Interface
