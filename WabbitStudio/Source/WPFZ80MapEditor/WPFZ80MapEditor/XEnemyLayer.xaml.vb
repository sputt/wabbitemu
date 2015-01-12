Public Class XEnemyLayer
    Inherits ZBaseLayer(Of ZEnemy)

    Public Overrides ReadOnly Property LayerType As LayerType
        Get
            Return WPFZ80MapEditor.LayerType.EnemyLayer
        End Get
    End Property

    Protected Overrides Function FinishDrop(Def As ZDef, Args As IList(Of Object))
        Return ZEnemy.FromDef(Def, Args)
    End Function

End Class
