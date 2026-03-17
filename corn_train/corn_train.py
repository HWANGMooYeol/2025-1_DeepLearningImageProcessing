from ultralytics import YOLO

def train():
    # Load a pretrained YOLO model
    model = YOLO('yolov8n.pt')

    # Train the model using the 'maskdataset.yaml' dataset for 3 epochs
    results = model.train(data='corndataset2.yaml', epochs=5)
    
if __name__ == '__main__':
    train()
    
