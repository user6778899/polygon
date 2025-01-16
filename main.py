# Импортируем нампай в np
import numpy as np

# Функция принимает обычный список
def sum_more2(arr):
    # Преобразуем массив в объект numpy
    arr = np.array(arr)
    
    # Считаем количество вхождений каждого элемента
    unique, counts = np.unique(arr, return_counts=True)
    #Параметр return_counts=True в функции np.unique нужен для того, чтобы вернуть
    #не только уникальные элементы массива, но и количество их вхождений.
    
    # Фильтруем элементы, которые встречаются более 2 раз
    elements = unique[counts > 2]
    
    # Суммируем эти элементы в исходном массиве
    result = np.sum(elements)

    # Возвращаем сумму
    return result

# Тесты
arr = [1,1,1,2]
print(sum_more2(arr))                               # 1

print(sum_more2([1,1,2,2,3,3,4,4,5,6,6]))           # 0

print(sum_more2([1,1,1,4,4,4,5,6,6,6]))             # 11

print(sum_more2([-1,-2,-2,-2,1,23,2,2,2]))          # 0

print(sum_more2([0,0,0,0,0,0,0,0,1]))               # 0

print(sum_more2([123,123,2,3,4,2,3,5,2,7,7,4,5,5])) # 7
