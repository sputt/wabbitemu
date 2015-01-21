Class Application

    Public Shared StartTime As Long

    ' Application-level events, such as Startup, Exit, and DispatcherUnhandledException
    ' can be handled in this file.

    Private Sub Application_Startup(sender As Object, e As StartupEventArgs)
        If e.Args.Length = 1 Then
            WPFZ80MapEditor.MainWindow.ZeldaFolder = e.Args(0)
        End If
        StartTime = Now.ToFileTime
    End Sub
End Class
