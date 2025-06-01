const chokidar = require('chokidar');
const fs = require('fs-extra');
const path = require('path');
const drivelist = require('drivelist');

const localFolder = path.resolve(__dirname, '../../arduino/computer/_SD');
const sdCardLabel = 'NET_WEAVING';

let sdCardPath = null;
let previousSdCardPath = null;

// 🔍 Find SD card mount point
async function findSdCardPath() {
    const drives = await drivelist.list();

    for (const drive of drives) {
        if (!drive.isRemovable || !drive.isCard || !drive.mountpoints.length) continue;

        for (const mount of drive.mountpoints) {
            if (mount.label && mount.label.toUpperCase() === sdCardLabel.toUpperCase()) {
                return mount.path;
            }
        }
    }

    console.log(`⚠️ SD card "${sdCardLabel}" not found or not mounted.`);
    return null;
}

// 📂 Sync a single file to SD card
async function syncFileToSD(filePath) {
    if (!sdCardPath) {
        console.warn(`⚠️ SD card not connected. Skipping: ${path.relative(localFolder, filePath)}`);
        return;
    }

    const relativePath = path.relative(localFolder, filePath);
    const destPath = path.join(sdCardPath, relativePath);

    try {
        await fs.copy(filePath, destPath, { overwrite: true });
        console.log(`📥 Synced file: ${relativePath}`);
    } catch (err) {
        console.warn(`❌ Failed to sync file: ${relativePath} - ${err.message}`);
    }
}

// 🔄 Sync the entire local folder to SD
async function syncAllToSD() {
    if (!sdCardPath) {
        console.warn('⚠️ SD card not connected. Full sync skipped.');
        return;
    }

    try {
        await fs.copy(localFolder, sdCardPath, { overwrite: true });
        console.log('✅ Full folder sync complete.');
    } catch (err) {
        console.error(`❌ Full sync failed: ${err.message}`);
    }
}

// 👀 Watch local folder for changes
const localWatcher = chokidar.watch(localFolder, { ignoreInitial: true });

localWatcher.on('add', (filePath) => {
    syncFileToSD(filePath);
});

localWatcher.on('change', (filePath) => {
    syncFileToSD(filePath);
});

// 🔁 Check for SD card connection
async function checkSdCard() {
    const foundPath = await findSdCardPath();

    if (foundPath && !sdCardPath) {
        console.log(`📀 SD card connected at ${foundPath}`);

        sdCardPath = foundPath;
        previousSdCardPath = foundPath;
        await fs.ensureDir(sdCardPath);
        await syncAllToSD(); // 🔁 Initial sync on detection
    } else if (!foundPath && sdCardPath) {
        console.log(`❎ SD card removed.`);
        sdCardPath = null;
        previousSdCardPath = null;
    } else if (foundPath && foundPath !== previousSdCardPath) {
        console.log(`🔁 SD card mount point changed: ${foundPath}`);
        sdCardPath = foundPath;
        previousSdCardPath = foundPath;
        await fs.ensureDir(sdCardPath);
        await syncAllToSD(); // 🔁 Re-sync on mount point change
    }
}

// 🕒 Poll for SD card changes every 2 seconds
setInterval(checkSdCard, 2000);

// ▶️ Run immediately at startup
checkSdCard();

console.log(`👁️ Watching local folder: ${localFolder}`);
