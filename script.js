// Select the form and response message container
const uploadForm = document.getElementById("uploadForm");
const responseMessage = document.getElementById("responseMessage");

// Add an event listener to handle form submission
uploadForm.addEventListener("submit", async (event) => {
  event.preventDefault(); // Prevent the default form submission

  const fileInput = document.getElementById("fileInput");
  const file = fileInput.files[0];

  if (!file) {
    responseMessage.textContent = "Please select a file to upload.";
    return;
  }

  // Create a FormData object to send the file
  const formData = new FormData();
  formData.append("file", file);

  try {
    // Send the file to the Master Node
    const response = await fetch("http://localhost:8080/upload", {
      method: "POST",
      body: formData,
    });

    if (response.ok) {
      const result = await response.text();
      responseMessage.textContent = "File uploaded successfully!";
      console.log("Response:", result);
    } else {
      responseMessage.textContent = "Error uploading file. Please try again.";
      console.error("Error:", response.statusText);
    }
  } catch (error) {
    responseMessage.textContent = "An error occurred. Please check your connection.";
    console.error("Error:", error);
  }
});
