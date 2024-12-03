import os
import wandb
import pytorch_lightning as pl
import torch

# Ensure the base URL is set to cloud
os.environ["WANDB_BASE_URL"] = "https://api.wandb.ai"

# Explicitly log in with your API key
wandb.login(key="bc8461c805307b06d488675dbe429eb7547a4395")

# Initialize the WandB logger
wandb_logger = pl.loggers.WandbLogger(
    project="Deep Learning Computer Vision",
    log_model="all"
)

# Set up a simple callback for the trainer
checkpoint_callback = pl.callbacks.ModelCheckpoint(monitor="val_acc", mode="max")

# Initialize the trainer
trainer = pl.Trainer(
    max_epochs=5,  # Set a small number for testing
    precision="16-mixed",
    accelerator="gpu",
    devices=1,
    logger=wandb_logger,
    callbacks=[checkpoint_callback],
)

# Dummy fit (since you may not have a model to train right now)
class DummyModel(pl.LightningModule):
    def training_step(self, batch, batch_idx):
        return torch.tensor(0.0)  # Dummy loss value

    def configure_optimizers(self):
        return torch.optim.Adam(self.parameters(), lr=1e-3)

dummy_model = DummyModel()

# Train the model (or run fit)
trainer.fit(dummy_model)

# Finish the W&B run
wandb.finish()
