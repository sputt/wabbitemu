Namespace SimConverters

    Public Class ShadowScalingConverter
        Inherits OneWayConverter(Of ZDrawEntry, Double)

        Public Overrides Function Convert(Entry As ZDrawEntry, parameter As Object) As Double
            If Entry.D = 0 Or Entry.Z = 0 Or (Entry.Flags And ZDrawEntry.D_NOSHADOW) Then
                Return 0.0
            End If

            Dim Area As Double = Math.Min(12 * 12, CDbl(Entry.W) * CDbl(Entry.H))
            Dim OverallScale = 0.3 + (0.7 * Area / (12 * 12))

            ' Z of 1 should be 1.0, Z of 8 should be 0.6
            Dim ZScale = 0.75 + 0.25 * ((7 - (Entry.Z - 1)) / 7)

            Return parameter * OverallScale * ZScale
        End Function
    End Class

    Public Class ImageConverter
        Inherits OneWayMultiValueConverter

        Public Overrides Function Convert(values() As Object, targetType As Type, parameter As Object, culture As Globalization.CultureInfo) As Object
            Dim GameModel As GameModel = TryCast(values(0), GameModel)
            If GameModel Is Nothing Then Return Nothing

            Dim Index As Integer = values(1)
            If Index > GameModel.Scenario.Images.Count Then
                Return Nothing
            ElseIf Index = 0 Then
                Return GameModel.Scenario.Images(Index).Image
            End If

            Dim Img As ZeldaImage = GameModel.Scenario.Images(Index)
            If Left(Img.Label, 8) = "LINK_GFX" Then
                If GameModel.GameFlags And GameModel.P_PUSHING Then
                    Dim PushName = "LINK_PUSH_GFX" & Right(Img.Label, 1)
                    Return GameModel.Scenario.Images.ToList().Find(Function(i) i.Label = PushName).Image
                ElseIf GameModel.GameFlags And GameModel.P_SHIELD Then
                    Dim ShieldName = "LINK_SHIELD_GFX" & Right(Img.Label, 1)
                    Return GameModel.Scenario.Images.ToList().Find(Function(i) i.Label = ShieldName).Image
                End If
            End If
            Return Img.Image
        End Function
    End Class

End Namespace
