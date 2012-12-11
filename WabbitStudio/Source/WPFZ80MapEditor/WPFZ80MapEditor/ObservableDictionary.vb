Public Class ObservableDictionary(Of TKey, TValue)
    Inherits Dictionary(Of TKey, TValue)
    Implements System.Collections.Specialized.INotifyCollectionChanged
    Implements System.ComponentModel.INotifyPropertyChanged

    Shadows Sub Add(Key As TKey, Val As TValue)
        MyBase.Add(Key, Val)

        Dim Args As New Specialized.NotifyCollectionChangedEventArgs(Specialized.NotifyCollectionChangedAction.Add, Val)
        RaiseEvent CollectionChanged(Me, Args)
    End Sub

    Shadows Sub Add(Item As KeyValuePair(Of TKey, TValue))
        Add(Item.Key, Item.Value)
    End Sub

    Shadows Sub Remove(Key As TKey)
        MyBase.Remove(Key)

        Dim Args As New Specialized.NotifyCollectionChangedEventArgs(Specialized.NotifyCollectionChangedAction.Remove, Key)
        RaiseEvent CollectionChanged(Me, Args)
    End Sub

    Shadows Sub Clear()
        Dim KeySet = (From t In Keys).ToArray()
        MyBase.Clear()
        For Each Key In KeySet
            Dim Args As New Specialized.NotifyCollectionChangedEventArgs(Specialized.NotifyCollectionChangedAction.Remove, Key)
            RaiseEvent CollectionChanged(Me, Args)
        Next
    End Sub

    Shadows Function GetEnumerator() As System.Collections.IEnumerator
        Return Values.GetEnumerator()
    End Function


    Public Event CollectionChanged(sender As Object, e As Specialized.NotifyCollectionChangedEventArgs) Implements Specialized.INotifyCollectionChanged.CollectionChanged

    Public Event PropertyChanged(sender As Object, e As ComponentModel.PropertyChangedEventArgs) Implements ComponentModel.INotifyPropertyChanged.PropertyChanged
End Class
