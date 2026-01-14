<?php
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *');

require_once __DIR__ . '/../includes/db.php';

$patient_id = $_GET['id'] ?? 0;

if (!$patient_id) {
    echo json_encode(['error' => 'Patient ID required']);
    exit;
}

try {
    $stmt = $pdo->prepare("
        SELECT * FROM sensor_readings 
        WHERE patient_id = ? 
        ORDER BY timestamp DESC 
        LIMIT 100
    ");
    $stmt->execute([$patient_id]);
    $readings = $stmt->fetchAll();
    
    echo json_encode(['success' => true, 'readings' => $readings]);
} catch (Exception $e) {
    echo json_encode(['success' => false, 'error' => $e->getMessage()]);
}
?>
