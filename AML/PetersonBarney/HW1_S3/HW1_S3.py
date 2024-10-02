# ===================== IMPORTS =====================
# By Nguyen Ba Phi (S3VYH3) with the help of ChatGPT

import numpy as np
import pandas as pd
from sklearn.preprocessing import StandardScaler, LabelEncoder
from sklearn.metrics import accuracy_score, classification_report
from sklearn.model_selection import GridSearchCV
from sklearn.neighbors import KNeighborsClassifier, NearestCentroid
from imblearn.over_sampling import SMOTE
import seaborn as sns
import matplotlib.pyplot as plt
import os

# ===================== DATA PREPARATION =====================

# Load the dataset (replace with your path)
data_folder = '/Users/killercookie/Documents/GitHub/School-Code/AML/PetersonBarney/HW1_S3/datasets'

# Initialize lists to store evaluation results
accuracies = []
reports = []

# List of file names
train_files = [f'train{i}.csv' for i in range(1, 5)]
test_files = [f'test{i}.csv' for i in range(1, 5)]

# Loop through all dataset pairs
for i in range(4):
    print(f"Processing dataset pair {i + 1}")

    # Load datasets
    train_path = os.path.join(data_folder, train_files[i])
    test_path = os.path.join(data_folder, test_files[i])

    train_df = pd.read_csv(train_path)
    test_df = pd.read_csv(test_path)

    # Remove leading stars from phoneme labels
    train_df['phoneme_in_ascii'] = train_df['phoneme_in_ascii'].str.replace(r'^\*', '', regex=True)
    test_df['phoneme_in_ascii'] = test_df['phoneme_in_ascii'].str.replace(r'^\*', '', regex=True)

    # Extract features and target for training and testing
    X_train = train_df[['F0', 'F1', 'F2', 'F3']]
    y_train = train_df['phoneme_in_ascii']
    X_test = test_df[['F0', 'F1', 'F2', 'F3']]
    y_test = test_df['phoneme_in_ascii']

    # Encode target labels to integers
    le = LabelEncoder()
    y_train_encoded = le.fit_transform(y_train)
    y_test_encoded = le.transform(y_test)

    # Check class distribution
    class_counts = np.bincount(y_train_encoded)
    print(f"Class Distribution (Train Set {i + 1}):", class_counts)

    # Dynamically adjust k_neighbors based on the smallest class size
    min_class_size = min(class_counts)
    smote_k_neighbors = min(3, min_class_size - 1) if min_class_size > 1 else 1  # Ensure k_neighbors > 0

    # If classes are imbalanced, apply SMOTE with dynamically adjusted k_neighbors
    smote = SMOTE(random_state=42, k_neighbors=smote_k_neighbors)
    X_train_resampled, y_train_resampled = smote.fit_resample(X_train, y_train_encoded)

    # Data Normalization
    scaler = StandardScaler()
    X_train_scaled = scaler.fit_transform(X_train_resampled)
    X_test_scaled = scaler.transform(X_test)

    # ===================== MODEL INITIALIZATION =====================

    # Initialize and Train Classifiers
    ncc = NearestCentroid()
    knn = KNeighborsClassifier(n_neighbors=5)

    # Train models
    ncc.fit(X_train_scaled, y_train_resampled)
    knn.fit(X_train_scaled, y_train_resampled)

    # ===================== MODEL EVALUATION =====================

    # Predictions
    y_pred_ncc = ncc.predict(X_test_scaled)
    y_pred_knn = knn.predict(X_test_scaled)

    # Combine predictions using majority voting
    final_pred = np.array([np.bincount([y_pred_ncc[j], y_pred_knn[j]]).argmax() for j in range(len(y_pred_ncc))])

    # Evaluate the performance
    accuracy_combined = accuracy_score(y_test_encoded, final_pred)
    report_combined = classification_report(y_test_encoded, final_pred, target_names=le.classes_)

    # Output the results
    # print(f'Combined NCC + KNN Accuracy for Test Set {i + 1}: {accuracy_combined}')
    # print(f'Classification Report for Test Set {i + 1}:\n{report_combined}')

    # Save the accuracy and report for this pair
    accuracies.append(accuracy_combined)
    reports.append(report_combined)

    # ===================== HYPERPARAMETER TUNING (KNN) =====================

    # Use one of the train sets for hyperparameter tuning
    param_grid_knn = {
        'n_neighbors': [3, 5, 7, 9],
        'weights': ['uniform', 'distance'],
    }

    # Perform GridSearchCV on KNN with one of the train/test sets (e.g., train1 and test1)
    grid_search_knn = GridSearchCV(KNeighborsClassifier(), param_grid_knn, cv=3, n_jobs=-1)
    grid_search_knn.fit(X_train_scaled, y_train_resampled)

    # Get the best KNN model
    best_knn = grid_search_knn.best_estimator_

    # Predictions with the best KNN model
    y_pred_best_knn = best_knn.predict(X_test_scaled)

    # Evaluate the performance
    accuracy_best_knn = accuracy_score(y_test_encoded, y_pred_best_knn)
    report_best_knn = classification_report(y_test_encoded, y_pred_best_knn, target_names=le.classes_)

    # Output the results
    print(f'Optimized KNN Accuracy: {accuracy_best_knn}')
    print(f'Classification Report for Test Set {i + 1}:\n{report_best_knn}')