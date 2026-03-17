import kagglehub

# Download latest version
path = kagglehub.dataset_download("techzizou/labeled-mask-dataset-yolo-darknet")

print("Path to dataset files:", path)
