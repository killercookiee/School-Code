# By Nguyen Ba Phi (S3VYH3) with the help of ChatGPT

import numpy as np
import pandas as pd
from sklearn.preprocessing import StandardScaler, LabelEncoder
from sklearn.metrics import accuracy_score, classification_report
from sklearn.model_selection import train_test_split, RandomizedSearchCV
from sklearn.ensemble import RandomForestClassifier
from sklearn.neighbors import KNeighborsClassifier, NearestCentroid
from imblearn.over_sampling import SMOTE
import seaborn as sns
import matplotlib.pyplot as plt

# Load the dataset (replace with your path)
data_path = '/Users/killercookie/Documents/GitHub/School-Code/AML/PetersonBarney/verified_pb.data'
columns = ['M/F/C', 'SPKR', 'Phoneme_Number', 'Phoneme_Ascii', 'F0', 'F1', 'F2', 'F3']
df = pd.read_csv(data_path, sep='\s+', names=columns)

# Extract features and target
X = df[['F0', 'F1', 'F2', 'F3']]
y = df['Phoneme_Ascii']

# Encode target labels to integers
le = LabelEncoder()
y_encoded = le.fit_transform(y)

# Check class distribution
class_counts = np.bincount(y_encoded)
print("Class Distribution:", class_counts)

# Visualize class distribution
sns.countplot(x=y_encoded)
plt.title('Class Distribution')
# plt.show()

# If classes are imbalanced, apply SMOTE
smote = SMOTE(random_state=42, k_neighbors=3)  # Set n_neighbors to 3
X_resampled, y_resampled = smote.fit_resample(X, y_encoded)

# Data Normalization
scaler = StandardScaler()
X_scaled = scaler.fit_transform(X_resampled)

# Split Data for All Train / All Test
X_train, X_test, y_train, y_test = train_test_split(X_scaled, y_resampled, test_size=0.2, random_state=42)

# Initialize and Train Classifiers
ncc = NearestCentroid()
ncc.fit(X_train, y_train)

knn = KNeighborsClassifier(n_neighbors=5)
knn.fit(X_train, y_train)

rf = RandomForestClassifier(n_estimators=100, random_state=42)
rf.fit(X_train, y_train)

# Predictions
y_pred_ncc = ncc.predict(X_test)
y_pred_knn = knn.predict(X_test)
y_pred_rf = rf.predict(X_test)

# Combine predictions using majority voting
final_pred = np.array([np.bincount([y_pred_ncc[i], y_pred_knn[i], y_pred_rf[i]]).argmax() for i in range(len(y_pred_ncc))])

# Evaluate the performance
accuracy_combined = accuracy_score(y_test, final_pred)
report_combined = classification_report(y_test, final_pred, target_names=le.classes_)

# Output the results
print(f'Combined NCC + KNN + Random Forest Accuracy: {accuracy_combined}')
print(f'Classification Report:\n{report_combined}')

# Hyperparameter tuning for Random Forest using Randomized Search
param_dist = {
    'n_estimators': [50, 100, 200],
    'max_depth': [None, 10, 20, 30],
    'min_samples_split': [2, 5, 10],
    'min_samples_leaf': [1, 2, 4],
    'bootstrap': [True, False]
}

# Create a RandomizedSearchCV object
random_search = RandomizedSearchCV(estimator=rf, param_distributions=param_dist,
                                   n_iter=10, cv=3, n_jobs=-1, verbose=2, random_state=42)

# Fit the RandomizedSearchCV
random_search.fit(X_train, y_train)

# Get the best parameters
best_rf_random = random_search.best_estimator_

# Predictions with the best model
y_pred_best_rf_random = best_rf_random.predict(X_test)

# Evaluate the performance
accuracy_best_rf_random = accuracy_score(y_test, y_pred_best_rf_random)
report_best_rf_random = classification_report(y_test, y_pred_best_rf_random, target_names=le.classes_)

# Output the results
print(f'Optimized Random Forest Accuracy (Randomized Search): {accuracy_best_rf_random}')
print(f'Classification Report:\n{report_best_rf_random}')





from sklearn.ensemble import VotingClassifier
from sklearn.model_selection import GridSearchCV

# Define the voting classifier
voting_clf = VotingClassifier(estimators=[
    ('ncc', ncc),
    ('knn', knn),
    ('rf', rf)],
    voting='hard')  # or 'soft' for probabilities

# Fit the voting classifier
voting_clf.fit(X_train, y_train)

# Predictions
y_pred_voting = voting_clf.predict(X_test)

# Evaluate the performance
accuracy_voting = accuracy_score(y_test, y_pred_voting)
report_voting = classification_report(y_test, y_pred_voting, target_names=le.classes_)

# Output the results
print(f'Voting Classifier Accuracy: {accuracy_voting}')
print(f'Classification Report:\n{report_voting}')

# Hyperparameter tuning for KNN using GridSearchCV
param_grid_knn = {
    'n_neighbors': [3, 5, 7, 9],
    'weights': ['uniform', 'distance'],
}

# Create GridSearchCV for KNN
grid_search_knn = GridSearchCV(KNeighborsClassifier(), param_grid_knn, cv=3, n_jobs=-1)
grid_search_knn.fit(X_train, y_train)

# Get the best KNN model
best_knn = grid_search_knn.best_estimator_

# Predictions with the best KNN model
y_pred_best_knn = best_knn.predict(X_test)

# Evaluate the performance
accuracy_best_knn = accuracy_score(y_test, y_pred_best_knn)
report_best_knn = classification_report(y_test, y_pred_best_knn, target_names=le.classes_)

# Output the results
print(f'Optimized KNN Accuracy: {accuracy_best_knn}')
print(f'Classification Report:\n{report_best_knn}')
