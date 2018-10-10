from sklearn.neural_network import MLPClassifier
from sklearn.datasets import load_breast_cancer
from sklearn.datasets import load_iris
from sklearn.model_selection import train_test_split

import numpy as np
float_formatter = lambda x: "%.2f" % x
np.set_printoptions(formatter={'float_kind':float_formatter})


iris = load_iris()
in_train, in_test, out_train, out_test = train_test_split(iris.data, iris.target)


mlp = MLPClassifier()
mlp.fit(in_train, out_train)

print('Training accuracy: {:.3f}'.format(mlp.score(in_train, out_train)))
print('Testing accuracy: {:.3f}'.format((mlp.score(in_test, out_test))))


fake_data = np.array([[1, 1, 1, 1]])
print(mlp.predict_proba(fake_data))
